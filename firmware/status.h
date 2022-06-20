#pragma once

#include <stdint.h>
#include "motor.h"

namespace TRex {
  enum class StatusFlags {
    OK          = 0x00,
    SHUTDOWN    = 0x01,
    TIMEOUT     = 0x02,
  };

  struct MotorStatus {
    uint8_t speed = 0;
    int16_t current = 0;
    Motor::Direction direction = Motor::Direction::FORWARD;
    bool braking = false;
  };

  struct BatteryStatus {
    double voltage = 0;
    double threshold = 0;
  };

  struct Status {
    StatusFlags flags = StatusFlags::OK;
    BatteryStatus battery;
    MotorStatus leftMotor;
    MotorStatus rightMotor;
  };
}
