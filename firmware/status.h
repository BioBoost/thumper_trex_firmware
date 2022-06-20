#pragma once

#include <stdint.h>
#include "motor.h"
#include "enum_flags.h"

namespace TRex {
  enum class StatusFlags {
    OK          = 0x00,
    TIMEOUT     = 0x01,
    BATTERY_LOW = 0x02,
    // Must be powers of 2 !
  };
  DEFINE_ENUM_FLAG_OPERATORS(StatusFlags);

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
