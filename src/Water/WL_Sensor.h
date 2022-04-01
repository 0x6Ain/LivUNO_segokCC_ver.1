#ifndef WL_SENSOR_H
#define WL_SENSOR_H
#include <Arduino.h>

enum water_level { WATER_LEVEL_LOW, WATER_LEVEL_ENOUGH, WATER_LEVEL_HIGH, WATER_LEVEL_ERROR };


class WLSensor
{
private:
    uint8_t highPinNum, lowPinNum;

public:
    WLSensor(uint8_t high, uint8_t low)
    {
        highPinNum = high;
        lowPinNum = low;
        pinMode(highPinNum, INPUT_PULLUP); // Set High if Sensor is not connected;
        pinMode(lowPinNum, INPUT_PULLUP);  // Set High if Sensor is not connected;
    }

    String getWaterLevel()
    {
        int lowSensor, highSensor = -1;

        lowSensor = digitalRead(lowPinNum);
        highSensor = digitalRead(highPinNum);

        if (lowSensor == 0 && highSensor == 0)
        {
            return "Low";
        }

        else if (lowSensor == 1 && highSensor == 1)
        {
            return "High";
        }

        else if (lowSensor == 0 && highSensor == 1)
        {
            return "Error";
        }

        else if (lowSensor == 1 && highSensor == 0)
        {
            return "Enough";
        }

        else
        {
            return "Error";
        }
    }

    water_level getWaterLevel_enum()
    {
        String current = getWaterLevel();
        if (current == "Low")
            return WATER_LEVEL_LOW;
        else if (current == "Enough")
            return WATER_LEVEL_ENOUGH;
        else if (current == "High")
            return WATER_LEVEL_HIGH;
        else
            return WATER_LEVEL_ERROR;
    }
};

#endif