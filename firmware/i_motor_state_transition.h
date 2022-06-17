#pragma once

#include "motor.h"
#include "motor_state.h"

namespace TRex {

  class IMotorStateTransition {

    public:
      virtual MotorState next(MotorState current, MotorState target) = 0;

  };

}