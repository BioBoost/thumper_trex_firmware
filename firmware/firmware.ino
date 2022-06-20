#include <Wire.h>
    // Warning: The Wire library implementation uses a 32 byte buffer, therefore any communication should be within this limit.
    // Exceeding bytes in a single transmission will just be dropped.

#include "trex_platform.h"
#include "status.h"
#include "debug.h"

#define I2C_ADDRESS                   0x20
#define I2C_FREQUENCY                 100000
#define STATUS_PACKET_START_BYTE      0xAB
#define COMMAND_PACKET_START_BYTE     0xBA

TRex::TRexPlatform trex;
TRex::Status status;

// We should probable not access the battery and motors directly
// as these talk to the hardware. An analogue value read can take
// some time. Thats why we just return the last read values.
void i2c_read_request_handler(void) {
  uint16_t batteryVoltage = (uint16_t)(status.battery.voltage * 100);

  uint8_t buffer[] = {
    STATUS_PACKET_START_BYTE,
    // Status Flags
    (uint8_t)(status.flags),
    // Battery
    highByte(batteryVoltage),
    lowByte(batteryVoltage),
    // Left Motor
    status.leftMotor.speed,
    (uint8_t)status.leftMotor.direction,
    highByte(status.leftMotor.current),
    lowByte(status.leftMotor.current),
    (uint8_t)status.leftMotor.braking,
    // Right Motor
    status.rightMotor.speed,
    (uint8_t)status.rightMotor.direction,
    highByte(status.rightMotor.current),
    lowByte(status.rightMotor.current),
    (uint8_t)status.rightMotor.braking,
    // Errors
    // TODO: Add i2c errors and such
    0x00      // ERRORS = NONE
  };

  Wire.write(buffer, sizeof(buffer));

  //errorFlags = TRex::ErrorFlags::NONE;        // Reset error flags once they are reported to the I2C master

  debug("Status Packet:");
  for (size_t i = 0; i < sizeof(buffer); i++) {
    debug(" 0x");
    debug(buffer[i], HEX);
  }
  debugln("----------------");
}

void setup() {
  TCCR2B = TCCR2B & B11111000 | B00000110;    // set timer 2 divisor to  256 for PWM frequency of 122.070312500 Hz
    // Checkout original code for more information about this

  Serial.begin(115200);
  Wire.begin(address);
  Wire.setClock(frequency);
  Wire.onReceive(i2c_read_request_handler);

  debugln("Starting TRex Motor Controller ...");

  trex.register_status_interface(&i2c_slave);
  trex.beep(2);
}

void loop() {
  trex.update();
  status = trex.status();

  // if (millis() > 6000) {
  //   debugln("Stopping motors");
  //   trex.stop();
  // }
}