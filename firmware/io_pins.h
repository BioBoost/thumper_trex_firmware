#pragma once

#define LEFT_MOTOR_BRAKE_PIN        4     //  D4 - left  motor brake        control    pin    HIGH = Brake 
#define LEFT_MOTOR_DIRECTION_PIN    2     //  D2 - left  motor direction    control    pin    HIGH = Forward   Low = Reverse
#define LEFT_MOTOR_PWM_PIN          3     //  D3 - left  motor pulse width  modulation pin    0 - 255          Speed and Brake
#define LEFT_MOTOR_CURRENT_PIN      6     //  A6 - left  motor current      monitor    pin    0 - 1023         -20A to +20A   

#define RIGHT_MOTOR_BRAKE_PIN        9    //  D9 - right motor brake        control    pin    HIGH = Brake 
#define RIGHT_MOTOR_DIRECTION_PIN    10   // D10 - right motor direction    control    pin    HIGH = Forward   Low = Reverse
#define RIGHT_MOTOR_PWM_PIN          11   // D11 - right motor pulse width  modulation pin    0 - 255          Speed and Brake 
#define RIGHT_MOTOR_CURRENT_PIN      7    //  A7 - right motor current      monitor    pin    0 - 1023         -20A to +20A   

#define BATTERY_VOLTAGE_PIN          3    //  A3 - battery voltage          1V = 33.57        30V = 1007
