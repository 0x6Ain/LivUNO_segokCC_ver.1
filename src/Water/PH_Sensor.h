#ifndef PH_SENSOR_H
#define PH_SENSOR_H
#include <Arduino.h>
#include <SoftwareSerial.h>

class PHSensor
{
    uint8_t analogPinNum;

public:
    PHSensor(uint8_t pin)
    {
        analogPinNum = pin;
    }

    float getPHAvg()
    {
        int buf[10], temp;
        unsigned long int avgValue;

        for (int i = 0; i < 10; i++) // Get 10 sample value from the sensor for smooth the value
        {
            buf[i] = analogRead(analogPinNum);
            delay(10);
        }
        for (int i = 0; i < 9; i++) // sort the analog from small to large
        {
            for (int j = i + 1; j < 10; j++)
            {
                if (buf[i] > buf[j])
                {
                    temp = buf[i];
                    buf[i] = buf[j];
                    buf[j] = temp;
                }
            }
        }
        avgValue = 0;
        for (int i = 2; i < 8; i++) // take the average value of 6 center sample
            avgValue += buf[i];
        float phValue = (float)avgValue * 5.0 / 1024 / 6; // convert the analog into millivolt
        phValue = 3.5 * phValue;
        return phValue; // convert the millivolt into pH value
    }
};

#endif
