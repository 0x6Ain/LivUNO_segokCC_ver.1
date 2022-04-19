#include "controller.h"

void Controller::begin(uint8_t pin)
{
    pinNumber = pin;
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, HIGH);
    isTurnOn = false;
};

void Controller::turnOn()
{
    countOfContinuousOperate++;
    if(!isTurnOn) {
        digitalWrite(pinNumber, LOW);
        isTurnOn = true;
    }
};

void Controller::turnOff()
{   
    countOfContinuousOperate = 0;
    if(isTurnOn) {
        digitalWrite(pinNumber,HIGH);
        isTurnOn = false;
    }
};

void Controller::operateTwoSeconds()
{
    turnOn();
    delay(2000);
    turnOff();
};

