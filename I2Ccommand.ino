
//------------------------------------------------------------------------------- Receive commands from I²C Master -----------------------------------------------
void I2Ccommand(int recvflag)     
{
  byte b;                                                                      // byte from buffer
  int i;                                                                       // integer from buffer
  
  do                                                                           // check for start byte
  {
    b=Wire.read();                                                             // read a byte from the buffer
    if(b!=startbyte || recvflag!=27) {
      errorflags = errorflags | START_BYTE;                 // if byte does not equal startbyte or Master request incorrect number of bytes then generate error
    }
  } while (errorflags>0 && Wire.available()>0);                                 // if errorflags>0 then empty buffer of corrupt data
  
  if(errorflags>0)                                                              // corrupt data received 
  {
    Shutdown();                                                                // shut down motors and servos
    return;                                                                    // wait for valid data packet
  }  
  //----------------------------------------------------------------------------- valid data packet received ------------------------------
  
  b=Wire.read();                                                               // read pwmfreq from the buffer
  if(b>0 && b<8)                                                               // if value is valid (1-7)  
  {
    leftmotor.pwm_frequency=b;                                                                 // update pwmfreq
    rightmotor.pwm_frequency=b;
    TCCR2B = TCCR2B & B11111000 | b;                                     // change timer 2 clock pre-scaler
  }
  else
  {
    errorflags = errorflags | PWM_FREQ;                                                 // incorrect pwmfreq given
  }
  
  i=Wire.read()*256+Wire.read();                                               // read integer from I²C buffer
  if(i>-256 && i<256)
  {
    leftmotor.speed=i;                                                                 // read new speed for   left  motor
  }
  else
  {
    errorflags = errorflags | MOTOR_SPEED;                                                 // incorrect motor speed given
  }
  leftmotor.brake=Wire.read();                                                         // read new left  motor brake status
  
  i=Wire.read()*256+Wire.read();                                               // read integer from I²C buffer
  if(i>-256 && i<256)
  {
    rightmotor.speed=i;                                                                 // read new speed for   right motor
  }
  else
  {
    errorflags = errorflags | MOTOR_SPEED;                                                 // incorrect motor speed given
  }
  rightmotor.brake=Wire.read();                                                         // read new right motor brake status
  
  if(errorflags & MOTOR_SPEED)                                                            // incorrect motor speed / shutdown motors 
  {
    leftmotor.speed=0;                                                                 // set left  motor speed to 0
    rightmotor.speed=0;                                                                 // set right motor speed to 0
  }
  
  i=Wire.read()*256+Wire.read();                                               // read integer from I²C buffer
  if(i>549 && i<3001)
  {
    battery.threshold=i;                                                                  // set low battery value (values higher than battery voltage will force a shutdown)
  }
  else
  {
    errorflags = errorflags | BATT_THRESHOLD;                                                // incorrect lowbat given
  }
  
  mode = ALLGOOD;                                                                      // breaks out of Shutdown mode when I²C command is given
  Motors();                                                                    // update brake, speed and direction of motors                                                                 // update servo positions
}
