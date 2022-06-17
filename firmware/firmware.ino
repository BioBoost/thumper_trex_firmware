#include "motor.h"
#include "io_pins.h"
#include "motor_controller.h"

#define _DEBUG_MODE

#ifdef _DEBUG_MODE
#define debug(text, ...) { Serial.print(text, ## __VA_ARGS__); }
#define debugln(text, ...) { Serial.println(text, ## __VA_ARGS__); }
#else
#define debug(format, ...) { }
#define debugln(format, ...) { }
#endif


// TRex::Motor leftMotor(LEFT_MOTOR_DIRECTION_PIN, LEFT_MOTOR_PWM_PIN, LEFT_MOTOR_BRAKE_PIN, LEFT_MOTOR_CURRENT_PIN);

void setup() {
  TCCR2B = TCCR2B & B11111000 | B00000110;    // set timer 2 divisor to  256 for PWM frequency of 122.070312500 Hz
    // Checkout original code for more information about this

#ifdef _DEBUG_MODE
  Serial.begin(9600);
#endif

  debugln("Starting TRex Motor Controller ...");

  TRex::Motor leftMotor(LEFT_MOTOR_DIRECTION_PIN, LEFT_MOTOR_PWM_PIN, LEFT_MOTOR_BRAKE_PIN, LEFT_MOTOR_CURRENT_PIN);
  TRex::MotorController leftController(leftMotor);
  
  leftController.drive(TRex::Motor::Direction::FORWARD, 80);
  delay(1000);
  leftController.stop();
}

void loop() {
  delay(100);
}