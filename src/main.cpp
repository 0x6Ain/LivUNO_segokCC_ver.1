#include "LivUno.h"
//2021-04-01 

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.print("LivFarm_Mega_ver.1");

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
  takeCurrentValue();
  // displayValuesInLCD();
  // Serial.println(payload);
  if (currentMinutes - controlECMinutes    >= controlECPeriod)     controlEC();
  if (currentMinutes - controlTempMinutes  >= controlTempPeriod)   controlTemp();
  if (currentMinutes - controlLedMinutes   >= controlLedPeriod)    controlLed();
  if (currentMinutes - controlHumidMinutes >= controlHumidPeriod)  controlHumid();
  
}
