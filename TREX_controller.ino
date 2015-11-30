
//=====================================================================================================================================//
//                     __________________   ____     ______________        __________________   _____         ______                   //
//                    /                 /  /    |   /              \      /                 /   \    \      _/    _/                   //
//                   /____      _______/  /_   /   /    _______     \    /    _____________/     \    \   _/    _/                     //
//                        /    /            | /   /    /      /     /   /    /_____               \    \_/    _/                       //
//                       /    /             |/   /    /______/     /   /          /              _/         _/                         //
//                      /    /                  /    ___     _____/   /    ______/             _/    _     /                           //
//                     /    /                  /    /   \    \       /    /____________      _/    _/ \    \                           //
//                    /    /                  /    /     \    \     /                 /    _/    _/    \    \                          //
//                   /____/                  /____/       \____\   /_________________/    /_____/       \____\                         //
//                                                                                                                                     //
//                 T'REX robot controller designed and programmed by Russell Cameron for DAGU Hi-Tech Electronics                      //
//=====================================================================================================================================//


#include <Wire.h>                                      // interrupt based I2C library
#include <Servo.h>                                     // library to drive up to 12 servos using timer1
#include <EEPROM.h>                                    // library to access EEPROM memory
#include "IOpins.h"                                    // defines which I/O pin is used for what function

// define constants here
#define startbyte 0x0F                                 // for serial communications each datapacket must start with this byte
#define _DO_DEBUG_

// Operational mode of the controller
enum OperationMode { ALLGOOD, SHUTDOWN, LOW_BATTERY };

struct I2cConfig {
  byte address;
  byte frequency;   // I2C clock frequency can be 100kHz(default) or 400kHz
};

struct Motor {
  byte pwm_frequency;   // value from 1-7
  int speed;            // motor speeds -255 to +255
  byte brake;           // brakes - non zero values enable brake
  int current;          // motor current
  int encoder;          // encoder values
};

struct Battery {
  int threshold;
  int voltage;      // Battery voltage*10 (accurate to 1 decimal place)
};

enum ErrorFlags {
  START_BYTE = 0x01,      // Start byte not received or incorrect data packet size.
  PWM_FREQ = 0x02,        // PWM frequency was not 1-7.
  MOTOR_SPEED = 0x04,     // Left or right motor speed was not -255 to +255.
  SERVO = 0x08,           // One or more servo positions was not -2400 to +2400.
  IMPACT = 0x10,          // Impact sensitivity not 0-1023.
  BATT_THRESHOLD = 0x20,  // Low battery was not 550 to 3000 (5.5V to 30V).
  I2C_ADDRESS = 0x40,     // I²C slave address was not 0-127.
  I2C_FREQ = 0x80         // I²C speed not 0 or 1 (100kHz or 400kHz)
};

// define global variables here
OperationMode mode = ALLGOOD;
Battery battery;
I2cConfig i2c_config;
Motor leftmotor;
Motor rightmotor;
byte errorflags;

int xaxis,yaxis,zaxis;                                 // X, Y, Z accelerometer readings
int deltx,delty,deltz;                                 // X, Y, Z impact readings 
int magnitude;                                         // impact magnitude
byte devibrate=50;                                     // number of 2mS intervals to wait after an impact has occured before a new impact can be recognized
int sensitivity=50;                                    // minimum magnitude required to register as an impact

byte RCdeadband=35;                                    // RCsignal can vary this much from 1500uS without controller responding
unsigned long time;                                    // timer used to monitor accelerometer and encoders

byte servopin[6]={7,8,12,13,5,6};                      // array stores IO pin for each servo
int servopos[6];                                       // array stores position data for up to 6 servos
Servo servo[6];                                        // create 6 servo objects as an array

void setup()
{
  //========================================== Choose your desired motor PWM frequency ================================================//
  //                       Note that higher frequencies increase inductive reactance and reduce maximum torque                         //
  //                               Many smaller motors will not work efficiently at higher frequencies                                 //
  //                      The default is 122Hz. This provides relatively low noise and relatively smooth torque                        //
  //                                    This setting can be changed using I2C or Bluetooth                                             //
  //                                                                                                                                   //
  //     Thanks to macegr - http://forum.arduino.cc/index.php?PHPSESSID=n1691l4esq4up52krpcb77bgm1&topic=16612.msg121031#msg121031     //
  //===================================================================================================================================//

  //TCCR2B = TCCR2B & B11111000 | B00000001; pwmfreq=1;    // set timer 2 divisor to    1 for PWM frequency of  31250.000000000 Hz
  //TCCR2B = TCCR2B & B11111000 | B00000010; pwmfreq=2;    // set timer 2 divisor to    8 for PWM frequency of   3906.250000000 Hz
  //TCCR2B = TCCR2B & B11111000 | B00000011; pwmfreq=3;    // set timer 2 divisor to   32 for PWM frequency of    976.562500000 Hz
  //TCCR2B = TCCR2B & B11111000 | B00000100; pwmfreq=4;    // set timer 2 divisor to   64 for PWM frequency of    488.281250000 Hz
  //TCCR2B = TCCR2B & B11111000 | B00000101; pwmfreq=5;    // set timer 2 divisor to  128 for PWM frequency of    244.140625000 Hz

  // Det timer 2 divisor to  256 for PWM frequency of 122.070312500 Hz
  TCCR2B = TCCR2B & B11111000 | B00000110;

  //TCCR2B = TCCR2B & B11111000 | B00000111; pwmfreq=7;    // set timer 2 divisor to 1024 for PWM frequency of     30.517578125 Hz

  // All IO pins are input by default on powerup
  // Configure motor control pins for output
  // pwm autoconfigures

  pinMode(lmpwmpin,OUTPUT);         // configure left  motor PWM       pin for output
  pinMode(lmdirpin,OUTPUT);         // configure left  motor direction pin for output
  pinMode(lmbrkpin,OUTPUT);         // configure left  motor brake     pin for output

  pinMode(rmpwmpin,OUTPUT);         // configure right motor PWM       pin for output
  pinMode(rmdirpin,OUTPUT);         // configure right motor direction pin for output
  pinMode(rmbrkpin,OUTPUT);         // configure right motor brake     pin for output
  
  // Config battery
  battery.threshold = 550;

  // Config i2c
  i2c_config.address = 0x07;
  i2c_config.frequency = 0;

  // Config Motor control
  leftmotor.pwm_frequency = 6;
  leftmotor.speed = 0;
  leftmotor.brake = 0;

  rightmotor.pwm_frequency = 6;
  rightmotor.speed = 0;
  rightmotor.brake = 0;

  // Clear all errors
  errorflags = 0;

  //----------------------------------------------------- Configure for I²C control ------------------------------------------------------
  MotorBeep(1);                                      // generate 1 beep from the motors to indicate I²C mode enabled
  byte i=EEPROM.read(0);                             // check EEPROM to see if I²C address has been previously stored
  if(i==0x55)                                        // B01010101 is written to the first byte of EEPROM memory to indicate that an I2C address has been previously stored
  {
    i2c_config.address = EEPROM.read(1);                       // read I²C address from EEPROM
  }
  else                                               // EEPROM has not previously been used by this program
  {
    EEPROM.write(0,0x55);                            // set first byte to 0x55 to indicate EEPROM is now being used by this program
    EEPROM.write(1,0x07);                            // store default I²C address
    i2c_config.address = 0x07;                       // set I²C address to default
  }
  
  Wire.begin(i2c_config.address);                    // join I²C bus as a slave at I2Caddress
  Wire.onReceive(I2Ccommand);                        // specify ISR for data received
  Wire.onRequest(I2Cstatus);                         // specify ISR for data to be sent

  #ifdef _DO_DEBUG_
  Serial.begin(9600);
  Serial.print("Starting TRex controller on address 0x");
  Serial.println(i2c_config.address, HEX);
  #endif
}


void loop()
{
  //----------------------------------------------------- Shutdown mode ----------------------------------------------------------------
  if (mode == SHUTDOWN || mode == LOW_BATTERY)    // if battery voltage too low
  {
    Shutdown();                                        // Shutdown motors and servos
    return;
  }
  
  //===================================================== Programmer's Notes ============================================================
  //                                    Detecting impacts requires reasonably accurate timing.                                         //
  //                  As all timers are in use this code uses the micros() function to simulate a timer interrupt.                     // 
  //                                                                                                                                   //
  //                      Reading an analog input takes 260uS so reading 3 analog inputs can take about 800uS.                         //
  //                 This code alternates between reading accelerometer data and voltage / current readings every 1mS.                 //
  //                  If you edit this code then be aware that impact detection may be affected if care is not taken.                  //
  //=====================================================================================================================================


  static byte alternate;                               // variable used to alternate between reading accelerometer and power analog inputs
  
  //----------------------------------------------------- Perform these functions every 1mS ---------------------------------------------- 
  if(micros()-time>999)                       
  {
    time=micros();                                     // reset timer
    alternate=alternate^1;                             // toggle alternate between 0 and 1
    Encoders();                                        // check encoder status every 1mS

    //--------------------------------------------------- These functions must alternate as they both take in excess of 780uS ------------    
    if(alternate)
    {
      Accelerometer();                                 // monitor accelerometer every second millisecond                            
    }
    else 
    {
      leftmotor.current = (analogRead(lmcurpin)-511)*48.83;     // read  left motor current sensor and convert reading to mA
      rightmotor.current = (analogRead(rmcurpin)-511)*48.83;    // read right motor current sensor and convert reading to mA
      
      battery.voltage = analogRead(voltspin)*10/3.357;          // read battery level and convert to volts with 2 decimal places (eg. 1007 = 10.07 V)
      if(battery.voltage < battery.threshold) {
        mode = LOW_BATTERY;                      // change to shutdown mode if battery voltage too low
      }
    }
  }
}
