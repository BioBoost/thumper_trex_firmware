#include "trex_platform.h"
#include "debug.h"

TRex::TRexPlatform trex;

void setup() {
  TCCR2B = TCCR2B & B11111000 | B00000110;    // set timer 2 divisor to  256 for PWM frequency of 122.070312500 Hz
    // Checkout original code for more information about this

  Serial.begin(115200);

  debugln("Starting TRex Motor Controller ...");

  // trex.drive(TRex::Motor::Direction::FORWARD, 80, TRex::Motor::Direction::BACKWARD, 80);
}

void loop() {

  trex.update();

  // if (millis() > 6000) {
  //   debugln("Stopping motors");
  //   trex.stop();
  // }
}