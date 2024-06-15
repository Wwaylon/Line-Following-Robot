#ifndef LINEFOLLOWING_H
#define LINEFOLLOWING_H

#define pwm_motor_channel_left 1
#define pwm_motor_channel_right 0
#define pwm_motor_res 8 //0-255 resolution
#define pwm_motor_freq 10000

#define MOTOR_SPEED 180

//Right motor
#define enableLeftMotor 26
#define leftMotorPin1 33
#define leftMotorPin2 32

//Left motor
#define enableRightMotor 17
#define rightMotorPin1 25
#define rightMotorPin2 13

#define ir_left_pin 4 
#define ir_right_pin 0 

#define trigPin  21
#define echoPin  16
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

void rotateMotor(int rightMotorSpeed, int leftMotorSpeed);

void line_following(int command); //line following logic. Place in while loop to use

void motors_init();


#endif