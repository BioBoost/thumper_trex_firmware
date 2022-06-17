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

      void drive(Motor::Direction direction, uint8_t speed) {
        this->control(direction, speed, false);
      }

      void brake(uint8_t power) {
        this->control(Motor::Direction::KEEP_CURRENT, power, true);
      }

      void stop(void) {
        this->control(Motor::Direction::KEEP_CURRENT, 0, false);
      }

    private:
      void control(Motor::Direction direction, uint8_t speed, bool braking) {
        MotorState current = { _motor.direction(), _motor.speed(), _motor.is_braking() };
        MotorState target = { direction, speed, braking };
        MotorState next = motorTransition->next(current, target);

        _motor.control(next.direction, next.speed, next.braking);
      }

    private:
      Motor & _motor;
      IMotorStateTransition * motorTransition;
      DirectMotorTransition defaultTransition;
  };

}