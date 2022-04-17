#include "LivUno.h"
#define DEBUG  //When Check about Serial printing, #define DEBUG
//2021-04-12 

void setup() 
{
  Serial.begin(9600);

  /* Setting Sensor */
  ecSensor.begin();
  temphumidSensor.begin();
  ppmSensor.begin();
  luxSensor.begin();

  /* Setting Control */
  aircon.begin(AIRCON_RELAY_PIN);
  fan.begin(FAN_RELAY_PIN);
  led.begin(LED_RELAY_PIN);
  nutrient.begin(NUTRIENT_RELAY_PIN);
  /* Checking Control */
  #ifdef DEBUG
  aircon.operateTwoSeconds();
  fan.operateTwoSeconds();
  led.operateTwoSeconds();
  nutrient.operateTwoSeconds();
  Serial.println("UnosetDone");
  #endif
  /* Wait Untill Arduino Wifi Set Done */
  // String temp = "";
  // while (strcmp(temp.c_str(), "wifiSetDone") != 0)
  // {
  //   Serial.println("unoSetDone");
  //   while (Serial.available() > 0)
  //   {
  //     temp = Serial.readStringUntil('\n');
  //   }
  // }
}

void loop() {
  // put your main code here, to run repeatedly:
  currentSeconds = millis()/1000;
  setRequestHandlerFromWifi();
  if ( autoModeOnNutrient && (currentSeconds - controlECSeconds    >= controlECPeriod))     controlEC();
  if ( autoModeOnAircon   && (currentSeconds - controlTempSeconds  >= controlTempPeriod))   controlTemp();
  if ( autoModeOnFan      && (currentSeconds - controlHumidSeconds >= controlHumidPeriod))  controlHumid();

}
