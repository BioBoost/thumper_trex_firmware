#pragma once

#include "io_pins.h"
#include "motor.h"
#include "motor_controller.h"
#include "delta_motor_transition.h"
#include "motor_state.h"
#include "battery.h"
#include "status.h"
#include "i_status_publish.h"

namespace TRex {

  #define UPDATE_TIME_MS  1
  #define BATTERY_START_THRESHOLD       7.0
  #define BEEP_DURATION_MS              200

  class TRexPlatform {

    public:
      TRexPlatform(void)
        : leftMotor(LEFT_MOTOR_DIRECTION_PIN, LEFT_MOTOR_PWM_PIN, LEFT_MOTOR_BRAKE_PIN, LEFT_MOTOR_CURRENT_PIN),
          rightMotor(RIGHT_MOTOR_DIRECTION_PIN, RIGHT_MOTOR_PWM_PIN, RIGHT_MOTOR_BRAKE_PIN, RIGHT_MOTOR_CURRENT_PIN),
          battery(BATTERY_VOLTAGE_PIN, BATTERY_START_THRESHOLD),
          leftController(leftMotor),
          rightController(rightMotor),
          deltaTransition(1)
        {
          leftController.motor_state_transition(&deltaTransition);
          rightController.motor_state_transition(&deltaTransition);
      }

      // Call this method as often as possible in the main loop()
      void update(void) {
        if ((micros()-lastUpdate) >= UPDATE_TIME_MS * 1000L) {
          leftController.update();
          rightController.update();
          lastUpdate = micros();
          refresh_status();
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

      void beep(uint8_t beeps) {
        leftMotor.stop();
        rightMotor.stop();
        
        for (int b = 0; b < beeps; b++) {
          for (int duration = 0; duration < 2*BEEP_DURATION_MS ; duration++) {
            leftMotor.forward(255);
            rightMotor.forward(255);
            delayMicroseconds(50);
            leftMotor.stop();
            rightMotor.stop();
            delayMicroseconds(200);       // Generates 2kHz tone

            leftMotor.backward(255);
            rightMotor.backward(255);
            delayMicroseconds(50);
            leftMotor.stop();
            rightMotor.stop();
            delayMicroseconds(200);       // Generates 2kHz tone
          }
          delay(BEEP_DURATION_MS);     // Pause between beeps
        }
      }

      void shutdown(void) {
        // status = StatusFlags::SHUTDOWN;
        leftMotor.stop();
        rightMotor.stop();
      }

      Status status(void) {
        return status;
      }

    private:
      void refresh_status(void) {
        Status status;

        status.flags = statusFlags;

        status.battery.voltage = battery.voltage();
        status.battery.threshold = battery.threshold();

        status.leftMotor.speed = leftMotor.speed();
        status.leftMotor.current = leftMotor.current();
        status.leftMotor.direction = leftMotor.direction();
        status.leftMotor.braking = leftMotor.is_braking();

        status.rightMotor.speed = rightMotor.speed();
        status.rightMotor.current = rightMotor.current();
        status.rightMotor.direction = rightMotor.direction();
        status.rightMotor.braking = rightMotor.is_braking();

        this->status = status;
      }

    private:
      Motor leftMotor;
      MotorController leftController;

      Motor rightMotor;
      MotorController rightController;

      DeltaMotorTransition deltaTransition;

      unsigned long lastUpdate = 0;
      // unsigned long lastControl;

      Battery battery;
      
      Status status;
      StatusFlags statusFlags = StatusFlags::OK;
  };

}