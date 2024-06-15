#ifndef BUTTONS_H
#define BUTTONS_H

#define buttonPin 22
extern bool setup_flag =false;
extern bool resume_flag =false;

void buttonPressed();

void sequenceEllapsed();

void buttonISR();

void buttons_init();


#endif