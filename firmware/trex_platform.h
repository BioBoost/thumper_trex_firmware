#pragma once

#include "io_pins.h"
#include "motor.h"
#include "motor_controller.h"
#include "delta_motor_transition.h"
#include "motor_state.h"

namespace TRex {

  #define UPDATE_TIME_MS  1

  class TRexPlatform {

    public:
      TRexPlatform(void)
        : leftMotor(LEFT_MOTOR_DIRECTION_PIN, LEFT_MOTOR_PWM_PIN, LEFT_MOTOR_BRAKE_PIN, LEFT_MOTOR_CURRENT_PIN),
          rightMotor(RIGHT_MOTOR_DIRECTION_PIN, RIGHT_MOTOR_PWM_PIN, RIGHT_MOTOR_BRAKE_PIN, RIGHT_MOTOR_CURRENT_PIN),
          leftController(leftMotor),
          rightController(rightMotor),
          deltaTransition(1)
        {
          // TODO: State trans should be stable first !
          // leftController.motor_state_transition(&deltaTransition);
          // rightController.motor_state_transition(&deltaTransition);
      }

      // Call this method as often as possible in the main loop()
      void update(void) {
        if ((micros()-lastUpdate) >= UPDATE_TIME_MS * 1000L) {
          leftController.update();
          rightController.update();
          lastUpdate = micros();
        }
      }

      void drive(Motor::Direction leftDirection, uint8_t leftSpeed, Motor::Direction rightDirection, uint8_t rightSpeed) {
        leftController.drive(leftDirection, leftSpeed);
        rightController.drive(rightDirection, rightSpeed);
      }

      void brake(uint8_t power) {
        leftController.brake(power);
        rightController.brake(power);
      }

      void stop(void) {
        leftController.stop();
        rightController.stop();
      }

    private:
      Motor leftMotor;
      MotorController leftController;

      Motor rightMotor;
      MotorController rightController;

      DeltaMotorTransition deltaTransition;

      unsigned long lastUpdate = 0;
      // unsigned long lastControl;
  };

}