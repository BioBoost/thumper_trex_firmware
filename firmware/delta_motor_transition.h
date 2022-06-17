#pragma once

#include <stdint.h>
#include "i_motor_state_transition.h"
#include "debug.h"

namespace TRex {

  class DeltaMotorTransition : public IMotorStateTransition {

    public:
      DeltaMotorTransition(uint8_t delta = 1)
        : _delta(delta) {
      }

    public:
      MotorState next(MotorState current, MotorState target) {
        // Currently only supporting speed control (no braking and such)

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
          // else target speed is reached

        }
        else {

          if (current.speed > 0) {
            speed = max(current.speed - _delta, 0);    // Still slowing down
          }
          else if (current.speed == 0) {    // Just switch directions
            direction = (current.direction == Motor::Direction::FORWARD ? Motor::Direction::BACKWARD : Motor::Direction::FORWARD);
            if (target.speed > current.speed) {
              speed = min(current.speed + _delta, target.speed);;   // We can also start speedup
            }
          }

        }

        debug("CurrentSpeed:");
        debug(current.speed);
        debug(",");
        debug("TargetSpeed:");
        debug(target.speed);
        debug(",");
        debug("CurrentDirection:");
        debug(((uint8_t)current.direction)*255);
        debug(",");
        debug("TargetDirection:");
        debugln(((uint8_t)target.direction)*255);

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