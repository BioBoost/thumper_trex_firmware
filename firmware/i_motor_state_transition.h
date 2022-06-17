#pragma once

#include "motor.h"

namespace TRex {

  struct MotorState {
    public:
      Motor::Direction direction;
      uint8_t speed;
      bool braking;
  }; 

  class IMotorStateTransition {

    public:
      virtual MotorState next(MotorState current, MotorState target) = 0;

  };

}