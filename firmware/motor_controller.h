#pragma once

#include "motor.h"
#include "i_motor_state_transition.h"
#include "direct_motor_transition.h"

namespace TRex {

  class MotorController {

    public:
      MotorController(Motor & motor)
        : _motor(motor) {
        
        motor_state_transition(&defaultTransition);
      }

      void motor_state_transition(IMotorStateTransition * motorTransition) {
        this->motorTransition = motorTransition;
      }

      // This method must be called on a regular interval !
      void update(void) {
        MotorState current = { _motor.direction(), _motor.speed(), _motor.is_braking() };
        MotorState next = motorTransition->next(current, target);

        _motor.control(next.direction, next.speed, next.braking);
      }

      void drive(Motor::Direction direction, uint8_t speed) {
        target = { direction, speed, false };
      }

      void brake(uint8_t power) {
        target = { _motor.direction(), power, true };
      }

      void stop(void) {
        target = { _motor.direction(), 0, false };
      }

    private:
      Motor & _motor;
      IMotorStateTransition * motorTransition;
      DirectMotorTransition defaultTransition;

      MotorState target = { _motor.direction(), 0, false };
  };

}