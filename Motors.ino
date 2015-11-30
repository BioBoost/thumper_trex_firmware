void Motors()
{
  digitalWrite(lmbrkpin,leftmotor.brake>0);                     // if left brake>0 then engage electronic braking for left motor
  digitalWrite(lmdirpin,leftmotor.speed>0);                     // if left speed>0 then left motor direction is forward else reverse
  analogWrite (lmpwmpin,abs(leftmotor.speed));                  // set left PWM to absolute value of left speed - if brake is engaged then PWM controls braking
  
  digitalWrite(rmbrkpin,rightmotor.brake>0);                     // if right brake>0 then engage electronic braking for right motor
  digitalWrite(rmdirpin,rightmotor.speed>0);                     // if right speed>0 then right motor direction is forward else reverse
  analogWrite (rmpwmpin,abs(rightmotor.speed));                  // set right PWM to absolute value of right speed - if brake is engaged then PWM controls braking
}

void MotorBeep(byte beeps)                              
{
  digitalWrite(lmbrkpin,0);                             // ensure breaks are off
  digitalWrite(rmbrkpin,0);
  
  for(int b=0;b<beeps;b++)                              // loop to generate multiple beeps
  {
    for(int duration=0;duration<400;duration++)         // generate 2kHz tone for 200mS
    {
      digitalWrite(lmdirpin,1);                         // drive left  motor forward
      digitalWrite(rmdirpin,1);                         // drive right motor forward
      digitalWrite(lmpwmpin,1);                         // left  motor at 100%
      digitalWrite(rmpwmpin,1);                         // right motor at 100%
      delayMicroseconds(50);                            // limit full power to 50uS
      digitalWrite(lmpwmpin,0);                         // shutdown left  motor
      digitalWrite(rmpwmpin,0);                         // shutdown right motor
      delayMicroseconds(200);                           // wait aditional 200uS to generate 2kHz tone
      
      digitalWrite(lmdirpin,0);                         // drive left  motor backward
      digitalWrite(rmdirpin,0);                         // drive right motor backward
      digitalWrite(lmpwmpin,1);                         // left  motor at 100%
      digitalWrite(rmpwmpin,1);                         // right motor at 100%
      delayMicroseconds(50);                            // limit full power to 50uS
      digitalWrite(lmpwmpin,0);                         // shutdown left  motor
      digitalWrite(rmpwmpin,0);                         // shutdown right motor
      delayMicroseconds(200);                           // wait aditional 200uS to generate 2kHz tone
    }
    delay(200);                                         // pause for 200mS (1/5th of a second) between beeps
  }
}
  


