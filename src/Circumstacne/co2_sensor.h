#ifndef CO2_SENSOR_H
#define CO2_SENSOR_H
#include <SoftwareSerial.h>
#include <MHZ19.h>

  //RX, TX // 두개의 모듈이 serial 통신을 하면, 꼬아서 연결해야된다. 따라서, 센서Rx값은 아두이노의 Tx로 Tx값은 아두이노 Rx로 연결해야한다.(연결시 주의)
class co2Sensor
{
private:
  SoftwareSerial co2serial;
  MHZ19 ppmSensor = MHZ19(&co2serial);

public:
  co2Sensor(uint8_t rxPin, uint8_t txPin) : co2serial(rxPin,txPin) {}
  

  void begin()
  {
    co2serial.begin(9600);
  }

  float getPPM()
  {
    co2serial.listen();
    MHZ19_RESULT response = ppmSensor.retrieveData();
    if (response == MHZ19_RESULT_OK)
    {
      return ppmSensor.getCO2();
    }
    else
    {
      return -1;
    }
  };
};

#endif
