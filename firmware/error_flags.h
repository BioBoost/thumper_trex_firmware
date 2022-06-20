#pragma once

namespace TRex {

  enum class ErrorFlags {
    NONE                = 0x00,     // No errors detected
    START_BYTE          = 0x01,     // Start byte not received or incorrect
    PACKET_SIZE         = 0x02,     // Number of bytes received is not correct
    MOTOR_DIRECTION     = 0x04,     // Motor direction was not 0, 1 or 2
    BATTERY_THRESHOLD   = 0x08,     // Low battery was not 550 to 3000 (5.5V to 30V).
  };

};