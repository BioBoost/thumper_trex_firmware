#pragma once

#include <stdint.h>

namespace TRex {

  class Motor {

    public: enum class Direction { FORWARD = 0x00, BACKWARD = 0x01 };     ///, KEEP_CURRENT = 0x02 

    public:
      Motor(int directionPin, int pwmPin, int brakePin, int currentPin) {
        this->directionPin = directionPin;
        this->pwmPin = pwmPin;
        this->brakePin = brakePin;
        this->currentPin = currentPin;

        pinMode(directionPin, OUTPUT);
        pinMode(pwmPin, OUTPUT);
        pinMode(brakePin, OUTPUT);
        pinMode(currentPin, INPUT);

        stop();
      }

      void drive(Direction direction, uint8_t speed) {
        control(direction, speed, false);
      }

      void brake(uint8_t power) {
        control(_direction, power, true);
      }

      void stop(void) {
        control(_direction, 0, false);
      }

      Direction direction(void) {
        return _direction;
      }

      uint8_t speed(void) {
        return _speed;
      }

      bool is_braking(void) {
        return _braking; 
      }

      int16_t current(void) const {
        return (analogRead(currentPin)-511)*48.83;    // Return current in mA
      }

      void control(Direction direction, uint8_t speed, bool brake) {
        _speed = speed;
        _direction = direction;
        _braking = brake;
        write();
      }

    private:
      void write(void) {
        digitalWrite(brakePin, _braking);
        digitalWrite(directionPin, (_direction == Direction::FORWARD));
        analogWrite(pwmPin, _speed);
      }

    private:
      int directionPin;
      int pwmPin;
      int brakePin;
      int currentPin;

      uint8_t _speed = 0;
      Direction _direction = Direction::FORWARD;
      bool _braking = false;
  };

};