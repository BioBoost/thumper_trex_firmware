
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
//                 Totally refactored and minified by Nico De Witte (nico.dewitte@vives.be)                                            //
//=====================================================================================================================================//


#include <Wire.h>             // interrupt based I2C library
#include "IOpins.h"           // defines which I/O pin is used for what function

// define constants here
#define startbyte 0x0F        // for serial communications each datapacket must start with this byte
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
};

struct Battery {
  int threshold;
  int voltage;      // Battery voltage*10 (accurate to 1 decimal place)
};

enum ErrorFlags {
  START_BYTE = 0x01,      // Start byte not received or incorrect data packet size.
  COMMAND_SIZE = 0x02,    // Number of bytes received is not correct
  MOTOR_SPEED = 0x04,     // Left or right motor speed was not -255 to +255.
  BATT_THRESHOLD = 0x08,  // Low battery was not 550 to 3000 (5.5V to 30V).
};

struct TRexStatusPacket {
  byte start;
  byte errorflags;
  int battery_voltage;
  int left_motor_current;
  int right_motor_current;
  byte operation_mode;
};

struct TRexCommandPacket {
  byte start;
  int left_motor_speed;
  byte left_motor_brake;
  int right_motor_speed;
  byte right_motor_brake;
  int battery_threshold;
};

// Define global variables here
OperationMode mode = ALLGOOD;
Battery battery;
I2cConfig i2c_config;
Motor leftmotor;
Motor rightmotor;
byte errorflags;
unsigned long time;         // Timer for analogue readouts
TRexStatusPacket status;    // Status of TRex controller (used for sending with i2c)
TRexCommandPacket command;  // For i2c command receival

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
  MotorBeep(3);                                      // generate 3 beeps from the motors to indicate I²C mode enabled
  
  Wire.begin(i2c_config.address);                    // join I²C bus as a slave at I2Caddress
  Wire.onReceive(I2Ccommand);                        // specify ISR for data received
  Wire.onRequest(I2Cstatus);                         // specify ISR for data to be sent

  #ifdef _DO_DEBUG_
  Serial.begin(9600);
  Serial.print("Starting TRex controller on address 0x");
  Serial.println(i2c_config.address, HEX);

  Serial.print("Status packet is ");
  Serial.print(sizeof(TRexStatusPacket));
  Serial.println(" bytes.");

  Serial.print("Command packet is ");
  Serial.print(sizeof(TRexCommandPacket));
  Serial.println(" bytes.");
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

  //----------------------------------------------------- Perform these functions every 2mS ---------------------------------------------- 
  if(micros()-time >= 2000)                       
  {
    time = micros();                                     // reset timer

    leftmotor.current = (analogRead(lmcurpin)-511)*48.83;     // read  left motor current sensor and convert reading to mA
    rightmotor.current = (analogRead(rmcurpin)-511)*48.83;    // read right motor current sensor and convert reading to mA
    
    battery.voltage = analogRead(voltspin)*10/3.357;          // read battery level and convert to volts with 2 decimal places (eg. 1007 = 10.07 V)
    if(battery.voltage < battery.threshold) {
      mode = LOW_BATTERY;                      // change to shutdown mode if battery voltage too low
    }
  }
}
