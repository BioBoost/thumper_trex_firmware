#define _DO_DEBUG_

void I2Cstatus()
{
  // Fill status packet
  status.start = startbyte;
  status.errorflags = errorflags;
  status.battery_voltage = battery.voltage;
  // status.left_motor_current = leftmotor.current;
  // status.left_motor_encoder = leftmotor.encoder;
  // status.right_motor_current = rightmotor.current;
  // status.right_motor_encoder = rightmotor.encoder;
  // status.accelerometer_x = xaxis;
  // status.accelerometer_y = yaxis;
  // status.accelerometer_z = zaxis;
  // status.impact_x = deltx;
  // status.impact_y = delty;
  // status.impact_z = deltz;
  
  // Data is send low byte first (Little Endian)
  Wire.write(((byte*)&status), sizeof(status));     // transmit data packet
  errorflags = 0;                                   // reset erroflag once error has been reported to I²C Master
}
