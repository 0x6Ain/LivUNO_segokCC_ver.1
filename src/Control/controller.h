#ifndef CONTROLLER_h
#define CONTROLLER_h
#include <Arduino.h>

class Controller
{
private:
    uint8_t pinNumber;
    
public:
    boolean isTurnOn;
    void begin(uint8_t pin);
    void turnOn();
    void turnOff();
    void operateTwoSeconds();
};

#endif
