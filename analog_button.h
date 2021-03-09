#ifndef ANALOG_BUTTON_H
#define ANALOG_BUTTON_H

// extern bool isButtonPressed ; // not used outside
extern byte buttonEvent;

// unsigned long lastButtonReadMs = 0;
// void setupAnalogButton();
void checkAnalogButton();
void service_command_button(); 

#endif