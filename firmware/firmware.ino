#include <Wire.h>
    // Warning: The Wire library implementation uses a 32 byte buffer, therefore any communication should be within this limit.
    // Exceeding bytes in a single transmission will just be dropped.

#include "trex_platform.h"
#include "status.h"
#include "debug.h"
#include "error_flags.h"

#define I2C_ADDRESS                   0x30
#define I2C_FREQUENCY                 100000
#define STATUS_PACKET_START_BYTE      0xAB
#define COMMAND_PACKET_START_BYTE     0xBA
#define COMMAND_PACKET_MAX_SIZE       6
enum class I2cCommand { DRIVE = 0x01, STOP = 0x02 };

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


// Note: This handler will lock up the device if System calls such as Particle.publish() are made within,
// due to interrupts being disabled for atomic operations during this handler.
// Do not overload this handler with extra function calls other than what is immediately required to receive I2C data.
// Post process outside of this handler.
void i2c_write_request_handler(int numberOfBytes) {

  // [ 0xBA 0x01 LeftDir LeftSpeed RightDir RightSpeed ]      // Speed command
  // [ 0xBA 0x02 ]                                            // Stop command
  
  TRex::ErrorFlags errors = TRex::ErrorFlags::NONE;

  uint8_t start = Wire.read();

  if (start != COMMAND_PACKET_START_BYTE) errors = TRex::ErrorFlags::START_BYTE;
  if (numberOfBytes != COMMAND_PACKET_MAX_SIZE) errors = TRex::ErrorFlags::PACKET_SIZE;

  // Not 100% sure if this is best way.
  // Read all remaining bytes but ignore them
  if (errors != TRex::ErrorFlags::NONE) {
    while (Wire.available()) { Wire.read(); };
    return;
  }

  // We can call trex drive methods and such here because the interaction
  // with the hardware is done inside the trex.update() method
  I2cCommand command = (I2cCommand)(Wire.read());
  switch(command) {
    case I2cCommand::DRIVE: {
      debug("Received DRIVE command");
      TRex::Motor::Direction leftDir = (TRex::Motor::Direction)(Wire.read());
      uint8_t leftSpeed = Wire.read();
      TRex::Motor::Direction rightDir = (TRex::Motor::Direction)(Wire.read());
      uint8_t rightSpeed = Wire.read();
      trex.drive(leftDir, leftSpeed, rightDir, rightSpeed);
      break;
    }
    case I2cCommand::STOP: {
      debug("Received STOP command");
      trex.stop();
      break;
    }
    default: {
      debug("Unknown command");
      // TODO: Set error flag
    }
  }

  // Not all commands process all packet bytes. Here we discard remaining bytes.
  while (Wire.available()) { Wire.read(); };
}


void setup() {
  TCCR2B = TCCR2B & B11111000 | B00000110;    // set timer 2 divisor to  256 for PWM frequency of 122.070312500 Hz
    // Checkout original code for more information about this

  Serial.begin(115200);
  Wire.begin(I2C_ADDRESS);
  Wire.setClock(I2C_FREQUENCY);
  Wire.onReceive(i2c_write_request_handler);
  Wire.onRequest(i2c_read_request_handler);

  debugln("Starting TRex Motor Controller ...");
  debug("I2C Address = 0x");
  debugln(I2C_ADDRESS, HEX);

  trex.beep(2);
}

void loop() {
  trex.update();
  status = trex.status();
}