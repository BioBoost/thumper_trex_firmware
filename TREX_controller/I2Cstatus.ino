#define _DO_DEBUG_

void I2Cstatus()
{
  // Fill status packet
  status.start = startbyte;
  status.errorflags = errorflags;
  status.battery_voltage = battery.voltage;
  status.left_motor_current = leftmotor.current;
  status.right_motor_current = rightmotor.current;
  status.operation_mode = mode;
  
  // Data is send low byte first (Little Endian)
  Wire.write(((byte*)&status), sizeof(status));     // transmit data packet
  errorflags = 0;                                   // reset error flag once error has been reported to I²C Master
}
