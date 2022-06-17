#pragma once

#include "motor.h"

namespace TRex {

  struct MotorState {
    public:
      Motor::Direction direction;
      uint8_t speed;
      bool braking;
  };

}