#ifndef WT_SENSOR_H
#define WT_SENSOR_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>


class WTSensor
{
private : 
  uint8_t tempPinNum;
  OneWire oneWire;

public :
  WTSensor(uint8_t pin) : oneWire(pin) {}
  DallasTemperature waterTempSensor = DallasTemperature(&oneWire);

  float getWaterTemperature()
  {
    waterTempSensor.requestTemperatures();
    float waterTemp = waterTempSensor.getTempCByIndex(0);
    if (waterTemp != DEVICE_DISCONNECTED_C)
    {
      return waterTemp;
    }
    else
    {
      return -1;
    }
  }
};

#endif
