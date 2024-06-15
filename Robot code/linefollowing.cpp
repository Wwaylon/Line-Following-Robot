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

void line_following(int command){
  static int  right_IR;
  static int  left_IR;
  static long duration;
  static float distanceCm;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED/2;
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);  
  if(distanceCm < 15)
  {
    command =0;
  }
  if(command == 1 )
  {
    right_IR = digitalRead(ir_right_pin);
    left_IR = digitalRead(ir_left_pin);
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
  }else
  {
    rotateMotor(0, 0);
  }
}


void motors_init()
{
  pinMode(ir_left_pin, INPUT);
  pinMode(ir_right_pin, INPUT);

  pinMode(enableRightMotor, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);

  pinMode(enableLeftMotor, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  ledcSetup(pwm_motor_channel_left, pwm_motor_freq, pwm_motor_res);
  ledcSetup(pwm_motor_channel_right, pwm_motor_freq, pwm_motor_res);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enableLeftMotor, pwm_motor_channel_left);
  ledcAttachPin(enableRightMotor, pwm_motor_channel_right);
}