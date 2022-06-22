#pragma once

#include <stdint.h>

namespace TRex {

  class Battery {
    
    public:
      Battery(int voltagePin, double thresholdVoltage) {
        this->pin = voltagePin;
        threshold(thresholdVoltage);
      }

      double voltage(void) const {
        return analogRead(pin) / (10 * 3.357);
      }

      void threshold(double voltage) {
        thresholdVoltage = voltage;
      }

      double threshold(void) {
        return thresholdVoltage;
      }

      bool is_threshold_exceeded(void) {
        return (voltage() < thresholdVoltage);
      }

    private:
      int pin;
      double thresholdVoltage;
  };

};