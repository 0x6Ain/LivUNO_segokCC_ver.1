#include <Arduino.h>
#include "LivUno.h"
//2021-03-29 11

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.print("LivFarm_Mega_ver.1");

  /* Setting Sensor */
  ecSensor.begin();
  waterTempSensor.set_TempSensor(WATER_TEMP_PIN);
  temphumidSensor.begin();
  ppmSensor.begin();

  /* Setting Control */
  led.begin(LED_RELAY_PIN);
  nutrient.begin(NUTRIENT_RELAY_PIN);
  aircon.begin(AIRCON_RELAY_PIN);
  airconFan.begin(AIRCON_FAN_RELAY_PIN);

  /* Checking Control */
  // AirconFan.operateTwoSeconds();
  // led.operateTwoSeconds();
  // nutrient.operateTwoSeconds();
  // aircon.operateTwoSeconds();
  Serial.println("Setting Done");
}

void loop() {
  // put your main code here, to run repeatedly:
  currentMinutes = millis()/60000;
  takeCurrentValue();
  displayValuesInLCD();
  if (currentMinutes - controlECMinutes    >= controlECPeriod)     controlEC();
  if (currentMinutes - controlTempMinutes  >= controlTempPeriod)   controlTemp();
  if (currentMinutes - controlLedMinutes   >= controlLedPeriod)    controlLed();
  if (currentMinutes - controlHumidMinutes >= controlHumidPeriod)  controlHumid();
  
}
