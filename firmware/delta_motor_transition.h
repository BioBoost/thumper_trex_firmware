#pragma once

#include <stdint.h>
#include "i_motor_state_transition.h"

namespace TRex {

  class DeltaMotorTransition : public IMotorStateTransition {

    public:
      DeltaMotorTransition(uint8_t delta = 1)
        : _delta(delta) {
      }

    public:
      MotorState next(MotorState current, MotorState target) {
        // Currently only supporting speed control (no braking and such)

        // Serial.print("Target Speed => ");
        // Serial.println(target.speed);
        // Serial.print("Target Direction => ");
        // Serial.println((uint8_t)target.direction);

        uint8_t speed = current.speed;
        Motor::Direction direction = current.direction;

        if (current.direction == target.direction) {

          // std::min and std::max for making sure not overshooting target
          if (target.speed > current.speed) {               // Speedup
            speed = min(current.speed + _delta, target.speed);
          }
          else if (target.speed < current.speed) {          // Slowdown
            speed = max(current.speed - _delta, target.speed);
          }
          // else target reached

        }
        
        // else {

        //   if (current.speed > 0) speed = current.speed - _delta;    // Still slowing down
        //   else if (current.speed == 0) {    // Just switch directions
        //     direction = (current.direction == Motor::Direction::FORWARD ? Motor::Direction::BACKWARD : Motor::Direction::FORWARD);
        //     if (target.speed > current.speed) speed = current.speed + _delta;   // We can also start speedup
        //   }

        // }

        // Serial.print("Next Speed => ");
        // Serial.println(speed);
        // Serial.print("Next Direction => ");
        // Serial.println((uint8_t)direction);

        return {
          direction,
          speed,
          target.braking
        };
      }

    private:
      uint8_t _delta = 1;

  };

}