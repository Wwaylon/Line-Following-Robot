#include <Arduino.h>
#include "linefollowing.h"
#include <stdio.h>
void rotateMotor(int rightMotorSpeed, int leftMotorSpeed)
{
  
  if (rightMotorSpeed < 0)
  {
    digitalWrite(rightMotorPin1,LOW);
    digitalWrite(rightMotorPin2,HIGH);    
  }
  else if (rightMotorSpeed > 0)
  {
    digitalWrite(rightMotorPin1,HIGH);
    digitalWrite(rightMotorPin2,LOW);      
  }
  else
  {
    digitalWrite(rightMotorPin1,LOW);
    digitalWrite(rightMotorPin2,LOW);      
  }

  if (leftMotorSpeed < 0)
  {
    digitalWrite(leftMotorPin1,LOW);
    digitalWrite(leftMotorPin2,HIGH);    
  }
  else if (leftMotorSpeed > 0)
  {
    digitalWrite(leftMotorPin1,HIGH);
    digitalWrite(leftMotorPin2,LOW);      
  }
  else 
  {
    digitalWrite(leftMotorPin1,LOW);
    digitalWrite(leftMotorPin2,LOW);      
  }
  ledcWrite(pwm_motor_channel_left, abs(leftMotorSpeed));
  ledcWrite(pwm_motor_channel_right, abs(rightMotorSpeed));
}

void line_following(){
  static int right_IR = digitalRead(ir_right_pin);
  static int left_IR = digitalRead(ir_left_pin);
  if (right_IR == 0 && left_IR == 0)
  {
    //straight
    rotateMotor(MOTOR_SPEED, MOTOR_SPEED);
  }else if(right_IR == 1 && left_IR == 0)
  {
    //turn right
    rotateMotor(MOTOR_SPEED, -MOTOR_SPEED);
  }else if(right_IR == 0 && left_IR == 1)
  {
    //turn left  
    rotateMotor(-MOTOR_SPEED, MOTOR_SPEED ); 
  }else 
  {
    //stop
    rotateMotor(0, 0);
  } 
}