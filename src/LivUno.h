#ifndef LIVMEGA_H
#define LIVMEGA_H

#include <BH1750.h>
#include <Adafruit_HTU21DF.h>
#include <LiquidCrystal_I2C.h>
#include "Control/controller.h"
#include "Water/EC_Sensor.h"

#include "Water/WL_Sensor.h"
#include "Water/WT_Sensor.h"
#include "Circumstacne/co2_sensor.h"
#include "DisplayLCD/changeLengthSetLCD.h"

/* Define pin Number */ 


#define WATER_HIGH_PIN            13   // DFROBOT SEN0204
#define WATER_LOW_PIN             12   // DFROBOT SEN0204
//                                11   // None
//                                11   // None
#define NUTRIENT_RELAY_PIN        A0   //  12 V
#define LED_RELAY_PIN              9   // LED      220 V 
#define AIRCON_FAN_RELAY_PIN       8   //      12 V
#define AIRCON_RELAY_PIN           7   // Nutrient 220 V
#define WATER_TEMP_PIN             6   // YWROBOT DS18B20 + SEN050007
#define EC_RX_PIN                  5   // Atlas EC Sensor
#define EC_TX_PIN                  4   //
#define CO2_RX_PIN                 3   // DFROBOT MHZ19
#define CO2_TX_PIN                 2   //  

/*Temp,Humid and Lux sensors are I2C Communicate*/

Controller airconFan, led, aircon, nutrient; 

ECSensor ecSensor(EC_RX_PIN,EC_TX_PIN);
WTSensor waterTempSensor;
WLSensor waterLevelSensor(WATER_HIGH_PIN,WATER_LOW_PIN);
Adafruit_HTU21DF temphumidSensor;
BH1750 luxSensor;
co2Sensor ppmSensor(CO2_RX_PIN,CO2_TX_PIN);

LiquidCrystal_I2C lcd(0x27,20,4);



// Timing
extern volatile unsigned long timer0_millis;       // Reset Time Value

unsigned long currentMinutes;                      // Snapshot of current time
unsigned long controlECMinutes;                    // Timer to check EC and control Nutrient Pump
unsigned long controlTempMinutes;                  // Timer to check Temperature and control Aircon
unsigned long controlHumidMinutes;                 // Timer to check Temperature and control Aircon
unsigned long controlLedMinutes;                   // Timer to check Temperature and control Aircon

unsigned long controlECPeriod    =   30;           // Time in Minutes between controling Electronic Conductivity
unsigned long controlTempPeriod  =    5;           // Time in Minutes between controling Temperature
unsigned long controlHumidPeriod =    5;           // Time in Minutes between controling Humiditiy
unsigned long controlLedPeriod   =    1;           // Time in Minutes between controling LED


unsigned long wifiWaitPeriod = 5000;               // Time to wait for sensor data if message.available


float currentTemp = -1;
float currentHumidity = -1;
float currentLux = -1;
float currentPPM = -1;
float currentWaterTemp = -1;
String currentWaterLevel = "Error";
float currentEC = -1;
float currentPH = -1;

float goalEC = 2.0;
float goalTemp = 24;
float goalHumid = 34;
unsigned long turnOnLEDPeriod   = 18 * 60;
unsigned long turnOffLEDPeriod  =  6 * 60;


String payload = String(currentTemp) + "," + String(currentHumidity)+ "," + String(currentLux)+ "," + String(currentPPM) + "," +  String(currentWaterTemp) + "," + currentWaterLevel  + "," + String(currentPH) + "," + String(currentEC) + "," ;


void takeCurrentValue();
void displayValuesInLCD();

void controlEC();
void controlTemp();
void controlHumid();
void controlLed();

void connectToESPWithMillisDelay(int delay);
void setRequestHandlerFromWifi();




void takeCurrentValue()
{
  currentTemp = temphumidSensor.readTemperature();          if(Serial.available() > 0) connectToESPWithMillisDelay(wifiWaitPeriod);
  currentHumidity = temphumidSensor.readHumidity();         if(Serial.available() > 0) connectToESPWithMillisDelay(wifiWaitPeriod);
  currentLux = luxSensor.readLightLevel();;                 if(Serial.available() > 0) connectToESPWithMillisDelay(wifiWaitPeriod);
  currentPPM = ppmSensor.getPPM();                          if(Serial.available() > 0) connectToESPWithMillisDelay(wifiWaitPeriod);
  currentWaterTemp = waterTempSensor.getWaterTemperature(); if(Serial.available() > 0) connectToESPWithMillisDelay(wifiWaitPeriod);
  currentWaterLevel = waterLevelSensor.getWaterLevel();     if(Serial.available() > 0) connectToESPWithMillisDelay(wifiWaitPeriod);
  currentEC = ecSensor.getEC();                             if(Serial.available() > 0) connectToESPWithMillisDelay(wifiWaitPeriod);
  //TODO: currentPH = phSensor.getPH();                             if(Serial.available() > 0) connectToESPWithMillisDelay(500);

  payload = String(currentTemp) + "," + String(currentHumidity)+ "," + String(currentLux)+ "," + String(currentPPM) + "," +  String(currentWaterTemp) + "," + currentWaterLevel  + "," + String(currentPH) + "," + String(currentEC) + "," ;

};



void displayValuesInLCD()
{
  lcd.clear();
  lcd.print("Temperature" + changeLengthSetLCD(currentTemp,9));
  lcd.setCursor(0,1);
  lcd.print("Humidity" + changeLengthSetLCD(currentHumidity,12));
  lcd.setCursor(0,2);
  lcd.print("Lux" + changeLengthSetLCD(currentLux,17));
  lcd.setCursor(0,3);
  lcd.print("Co2" + changeLengthSetLCD(currentPPM,17));
  
  connectToESPWithMillisDelay(3000);

  lcd.clear();
  lcd.print("WaterTemp" + changeLengthSetLCD(currentWaterTemp,11));
  lcd.setCursor(0,1);
  lcd.print("WaterLevel" + changeLengthSetLCD(currentWaterLevel,10));
  lcd.setCursor(0,2);
  lcd.print("EC" + changeLengthSetLCD(currentEC,18));
  lcd.setCursor(0,3);
  lcd.print("PH" + changeLengthSetLCD(currentPH,18));

  connectToESPWithMillisDelay(3000);
};

void controlEC()
{
  Serial.println("Control EC Start");
   if (currentEC < goalEC && waterLevelSensor.getWaterLevel_enum() > WATER_LEVEL_LOW)
  {
    float difference = currentEC - goalEC;
    nutrient.turnOn();
    connectToESPWithMillisDelay(1000 * difference);
    nutrient.turnOff();
  }

  controlECMinutes = millis()/60000;
};

void controlTemp()
{
  Serial.println("Control Temp Start");
  if (currentTemp > goalTemp) aircon.turnOn();  
  else                        aircon.turnOff();
  controlTempMinutes = millis() /60000;
};

void controlHumid()
{
  if(currentHumidity > goalHumid) airconFan.turnOn();
  else                            airconFan.turnOff();
  controlHumidMinutes = millis()/60000;
};

void controlLed()
{
  if (!led.isTurnOn) {
    led.turnOn();
    controlLedPeriod = turnOnLEDPeriod;
  }

  else
  {
    led.turnOff();
    controlLedPeriod = turnOffLEDPeriod;
  }
  controlLedMinutes = millis()/60000;
};

void connectToESPWithMillisDelay(int delay)
{
  unsigned long currentMillis = millis();

  while (millis() < currentMillis + delay)
  {
    setRequestHandlerFromWifi();
  }
};



void setRequestHandlerFromWifi()
{ 
    String temp = "";
    while(Serial.available() > 0) 
    {
        temp = Serial.readStringUntil('\n');
        Serial.println(temp);

        char deliv[strlen(temp.c_str())];
        strcpy(deliv, temp.c_str());
        char* perform = strtok(deliv, "/");
        char* option = strtok(NULL,"/");
        char* ptr_p = strtok(NULL, "=");

        // Serial.println(perform);
        // Serial.println(option);

             if(strcmp(perform,"current") == 0)
        {
          Serial.print(payload);
          Serial.print("\n");
        }

        else if(strcmp(perform,"control") == 0)
        {
               if(strcmp(option,"reset") == 0)              asm volatile("jmp 0");
          else if(strcmp(option,"turnOnLED") == 0)          led.turnOn();
          else if(strcmp(option,"turnOffLED") == 0)         led.turnOff();
          else if(strcmp(option,"turnOnAirconFan") == 0)    airconFan.turnOn();
          else if(strcmp(option,"turnOffAirconFan") == 0)   airconFan.turnOff();
          else if(strcmp(option,"turnOnAircon") == 0)       aircon.turnOn();
          else if(strcmp(option,"turnOffAircon") == 0)      aircon.turnOff();
          else if(strcmp(option,"abjustEC") == 0)           controlEC();
          else if(strcmp(option,"abjustHumid") == 0)        controlHumid();
        }

        else if(strcmp(perform,"setting") == 0)
        {
               if(strcmp(option,"goalEC") == 0)             goalEC = atof(ptr_p);
          else if(strcmp(option,"goalTemp") == 0)           goalTemp = atof(ptr_p);
          else if(strcmp(option,"goalHumid") == 0)          goalHumid = atof(ptr_p);
          else if(strcmp(option,"controlECPeriod") == 0 )   controlECPeriod = atof(ptr_p);
          else if(strcmp(option,"controlTempPeriod") == 0 ) controlTempPeriod = atof(ptr_p);
          else if(strcmp(option,"controlHumidPeriod") == 0 )controlHumidPeriod = atof(ptr_p);
          else if(strcmp(option,"controlLedPeriod") == 0 )  controlLedPeriod = atof(ptr_p);
          else if(strcmp(option,"turnOnLEDPeriod") == 0 )   turnOnLEDPeriod = atof(ptr_p);
          else if(strcmp(option,"turnOffLEDPeriod") == 0 )  turnOffLEDPeriod = atof(ptr_p);

        }   
    }
}

#endif
