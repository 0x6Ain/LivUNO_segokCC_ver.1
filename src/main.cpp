#include "LivUno.h"
//2021-04-01 

void setup() {
  Serial.begin(9600);

  /* Setting Sensor */
  ecSensor.begin();
  temphumidSensor.begin();
  ppmSensor.begin();
  luxSensor.begin();

  /* Setting Control */
  aircon.begin(AIRCON_RELAY_PIN);
  fan.begin(AIRCON_FAN_RELAY_PIN);
  led.begin(LED_RELAY_PIN);
  nutrient.begin(NUTRIENT_RELAY_PIN);

  /* Checking Control */
  // aircon.operateTwoSeconds();
  // fan.operateTwoSeconds();
  // led.operateTwoSeconds();
  // nutrient.operateTwoSeconds();
 

Serial.println("Setting Done");
}

void loop() {
  // put your main code here, to run repeatedly:
  currentSeconds = millis()/1000;
  setRequestHandlerFromWifi();
  if ( isTurnOnNutrient && (currentSeconds - controlECSeconds    >= controlECPeriod))     controlEC();
  if ( isTurnOnAircon   && (currentSeconds - controlTempSeconds  >= controlTempPeriod))   controlTemp();
  if ( isTurnOnFan      && (currentSeconds - controlHumidSeconds >= controlHumidPeriod))  controlHumid();
  
}
