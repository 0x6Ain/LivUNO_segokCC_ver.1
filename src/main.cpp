#include "LivUno.h"
//2021-04-01 

bool isTurnOnAircon = false;
bool isTurnOnFan = false;
bool isTurnOnNutrient = false;

void setup() {
  Serial.begin(9600);

  /* Setting Sensor */
  ecSensor.begin();
  temphumidSensor.begin();
  ppmSensor.begin();
  luxSensor.begin();

  /* Setting Control */
  aircon.begin(AIRCON_RELAY_PIN);
  airconFan.begin(AIRCON_FAN_RELAY_PIN);
  led.begin(LED_RELAY_PIN);
  nutrient.begin(NUTRIENT_RELAY_PIN);

  /* Checking Control */
  aircon.operateTwoSeconds();
  airconFan.operateTwoSeconds();
  led.operateTwoSeconds();
  nutrient.operateTwoSeconds();

  Serial.println("Setting Done");
}

void loop() {
  // put your main code here, to run repeatedly:
  currentMinutes = millis()/60000;
  setRequestHandlerFromWifi();
  if ( isTurnOnNutrient && (currentMinutes - controlECMinutes    >= controlECPeriod))     controlEC();
  if ( isTurnOnAircon   && (currentMinutes - controlTempMinutes  >= controlTempPeriod))   controlTemp();
  if ( isTurnOnFan      && (currentMinutes - controlHumidMinutes >= controlHumidPeriod))  controlHumid();
  
}
