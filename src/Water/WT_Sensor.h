#ifndef WT_SENSOR_H
#define WT_SENSOR_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>


class WTSensor
{
private : 
  uint8_t tempPinNum;

public :
  void set_TempSensor(uint8_t pin) { tempPinNum = pin;}
  OneWire oneWire = OneWire(tempPinNum);
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
