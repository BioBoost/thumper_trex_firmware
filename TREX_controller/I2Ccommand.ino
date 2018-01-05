void I2Cerror(void) {
  byte fake;

  Shutdown();   // Shut down the motors

  while (Wire.available() > 0) {
    fake = Wire.read();
  }
}


void I2Ccommand(int recvflag)     
{
  // Check if command size is ok
  if (recvflag != sizeof(TRexCommandPacket)) {
    errorflags = errorflags | COMMAND_SIZE;
    I2Cerror();
    return;
  }

  // Read buffer into command struct
  for (byte i = 0; i < sizeof(TRexCommandPacket); i++) {
    ((byte*)&command)[i] = Wire.read();
  }

  // Check startbyte
  if (command.start != startbyte) {
    errorflags = errorflags | START_BYTE;
    I2Cerror();
    return;
  }

  if (command.left_motor_speed < -256 ||
    command.left_motor_speed > 256) {

      errorflags = errorflags | MOTOR_SPEED;
      leftmotor.speed = 0;     // set left  motor speed to 0
      rightmotor.speed = 0;    // set right motor speed to 0
      return;
  }

  if (command.right_motor_speed < -256 ||
    command.right_motor_speed > 256) {

      errorflags = errorflags | MOTOR_SPEED;
      leftmotor.speed = 0;     // set left  motor speed to 0
      rightmotor.speed = 0;    // set right motor speed to 0
      return;
  }

  if(command.battery_threshold < 549 && command.battery_threshold > 3001) {
    errorflags = errorflags | BATT_THRESHOLD;    
    return; 
  }

  // Use new configuration
  leftmotor.speed = command.left_motor_speed;
  leftmotor.brake = command.left_motor_brake;
  rightmotor.speed = command.right_motor_speed;
  rightmotor.brake = command.right_motor_brake;

  battery.threshold = command.battery_threshold;

  #ifdef _DO_DEBUG_
  Serial.print("L-speed: ");
  Serial.println(leftmotor.speed);
  Serial.print("R-speed: ");
  Serial.println(rightmotor.speed);
  Serial.print("Battery threshold: ");
  Serial.println(command.battery_threshold);
  #endif

  mode = ALLGOOD;           // breaks out of Shutdown mode when I²C command is given
  Motors();                 // update brake, speed and direction of motors                                                                 // update servo positions
}
