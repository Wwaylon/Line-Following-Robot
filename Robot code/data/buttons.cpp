#include <Arduino.h>
#include <EasyButton.h>
#include "buttons.h"
#include "globals.h"

EasyButton button(buttonPin);

void buttonPressed()
{
  resume_flag =true;
}

void sequenceEllapsed()
{
  setup_flag =true;
}
void buttonISR()
{
  button.read();
}


void  buttons_init()
{
  button.begin();
  button.onPressed(buttonPressed);
  button.onSequence(3, 500, sequenceEllapsed);
  if (button.supportsInterrupt())
  {
    button.enableInterrupt(buttonISR);
    Serial.println("Button will be used through interrupts");
  }
}