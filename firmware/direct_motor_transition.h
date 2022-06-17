#pragma once

#include "i_motor_state_transition.h"

namespace TRex {

  class DirectMotorTransition : public IMotorStateTransition {

    public:
      MotorState next(MotorState current, MotorState target) {
        return {
          target.direction,
          target.speed,
          target.braking
        };
      }

  };

}