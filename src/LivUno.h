#ifndef LIVUNO_H
#define LIVUNO_H

#include <BH1750.h>
#include <Adafruit_HTU21DF.h>
#include "Controller/controller.h"
#include "Water/EC_Sensor.h"
#include "Water/PH_Sensor.h"
#include "Water/WL_Sensor.h"
#include "Water/WT_Sensor.h"
#include "Circumstacne/co2_sensor.h"

//* Define pin Number */ /
#define WATER_LOW_PIN             13   // DFROBOT SEN0204
#define WATER_HIGH_PIN            12   // DFROBOT SEN0204
//                                11   // None
//                                10   // None
#define NUTRIENT_RELAY_PIN        A0   //  12 V
#define FAN_RELAY_PIN              9   //      12 V
#define AIRCON_RELAY_PIN           8   // Nutrient 220 V
#define LED_RELAY_PIN              7   // LED      220 V 
#define WATER_TEMP_PIN             6   // YWROBOT DS18B20 + SEN050007
#define EC_TX_PIN                  5   // Atlas EC Sensor
#define EC_RX_PIN                  4   // 
#define CO2_TX_PIN                 3   // DFROBOT MHZ19 
#define CO2_RX_PIN                 2   // 
#define PH_SENSOR_PIN             A2   // DFROBOT PH Sensor

/*Temp,Humid and Lux sensors are I2C Communicate*/

//* Uno and UnoWifi communicate Key *//
#define CURRENT_KEY "current"
#define SETTING_KEY "setting"
#define STATUS_KEY "status"
#define SWITCHES_KEY "switches"
#define SWITCHES_STATUS_KEY "switchesStatus"
#define ERROR_KEY "error"
#define SNAPSHOTS_KEY "snapshots"

Controller fan, led, aircon, nutrient; 
ECSensor ecSensor(EC_RX_PIN,EC_TX_PIN);
PHSensor phSensor(PH_SENSOR_PIN);
WTSensor waterTempSensor(WATER_TEMP_PIN);
WLSensor waterLevelSensor(WATER_HIGH_PIN,WATER_LOW_PIN);
Adafruit_HTU21DF temphumidSensor;
BH1750 luxSensor(0x23);
co2Sensor ppmSensor(CO2_RX_PIN,CO2_TX_PIN);

//TODO: set the standard of Limitaion Controller 
//* Limits of Controller *//

const byte limitsOfAircon   = 48;   //If aircon   continuously operate 10 times(5mins-> (12*4)), Error message to Log
const byte limitsOfFan      = 48;   //If Fan      continuously operate 10 times(5mins-> (12*4)), Error message to Log  
const byte limitsOfNutrient = 12;   //If Nutrient continuously operate 10 times(30mins-> (2*6)), Error message to Log

boolean airconError   = false;
boolean fanError      = false;
boolean nutrientError = false;

//* Controlller AutoMode Flag*//
bool autoModeOnAircon   = true;
bool autoModeOnFan      = true;
bool autoModeOnNutrient = true;

//* Time Values *//
extern volatile unsigned long timer0_millis;       // Reset Time Value
unsigned long currentSeconds;                      // Snapshot of current time
unsigned long controlECSeconds;                    // Timer to check EC and control Nutrient Pump
unsigned long controlTempSeconds;                  // Timer to check Temperature and control Aircon
unsigned long controlHumidSeconds;                 // Timer to check Temperature and control Aircon
unsigned long errorSeconds;                        // Timer to error
unsigned long controlECPeriod    =   30*60;        // Time in 30 * 60 Seconds between controling Electronic Conductivity
unsigned long controlTempPeriod  =    5*60;        // Time in  5 * 60 Seconds between controling Temperature
unsigned long controlHumidPeriod =    5*60;        // Time in  5 * 60 Seconds between controling Humiditiy
unsigned long errorPeriod         =  60*60;        // Time in   60*60 Seconds

//* Current Values *//
float currentTemp = -1;
float currentHumidity = -1;
int currentLux = -1;
int currentPPM = -1;
float currentWaterTemp = -1;
water_level currentWaterLevel = WATER_LEVEL_ERROR;
float currentEC = -1;
float currentPH = -1;

//* Desired Values *//
float goalEC = 2; 
int goalTemp = 24;
int goalHumid = 60;

String payload = String(currentTemp) + "," + String(currentHumidity)+ "," + String(currentLux)+ "," + String(currentPPM) + "," +  String(currentWaterTemp) + "," + currentWaterLevel  + "," + String(currentPH) + "," + String(currentEC);


void getCurrentValue();
void controlEC();
void controlTemp();
void controlHumid();
void controlLed();
void connectToUnoWifiWithMillisDelay(int delay);
void setRequestHandlerFromWifi();




void getCurrentValue()
{
  currentTemp = temphumidSensor.readTemperature();           
  currentHumidity = temphumidSensor.readHumidity();          
  currentLux = luxSensor.readLightLevel();                  
  currentPPM = ppmSensor.getPPM();                           
  currentWaterTemp = waterTempSensor.getWaterTemperature();  
  currentWaterLevel = waterLevelSensor.getWaterLevel_enum(); 
  currentEC = ecSensor.getEC();                              
  currentPH = phSensor.getPHAvg();                           

  // payload = String(currentTemp) + "," + String(currentHumidity)+ "," + String(currentLux)+ "," + String(currentPPM) + "," +  String(currentWaterTemp) + "," + String(currentWaterLevel)  + "," + String(currentPH) + "," + String(currentEC);
  payload = String(currentTemp) + "," + String(currentHumidity)+ "," + String(currentLux)+ "," + String(currentPPM) + "," +  String(currentWaterTemp) + "," + String(currentWaterLevel)  + "," + String(currentPH) + "," + String(currentEC);

};


void controlEC()
{
  currentEC = constrain(ecSensor.getEC(),0,10);
  if(!nutrientError && nutrient.countOfContinuousOperate <= limitsOfNutrient)
  {
    if (currentEC < goalEC && waterLevelSensor.getWaterLevel_enum() > WATER_LEVEL_LOW)
    {
      float difference = currentEC - goalEC;
      Serial.print("setEC:");
      Serial.print(nutrient.countOfContinuousOperate);
      Serial.print(" time");
      Serial.print("\n");
      nutrient.turnOn();
      connectToUnoWifiWithMillisDelay(1000 * difference);
      nutrient.turnOff();
    }
  }
  else 
  {
    nutrientError = true;
  }
  controlECSeconds = millis() /  1000;
};

void controlTemp()
{
  currentTemp = temphumidSensor.readTemperature();
  if (!airconError && aircon.countOfContinuousOperate <= limitsOfAircon)
  {
    if (currentTemp > goalTemp)
    {
      Serial.println("airconOn");
      aircon.turnOn();
    }
    else
    {
      Serial.println("airconOff");
      aircon.turnOff();
    }
  }
  else
  {
    airconError = true;
  }
  controlTempSeconds = millis() / 1000;
};

void controlHumid()
{
  
  currentHumidity = temphumidSensor.readHumidity();
  if (!fanError && fan.countOfContinuousOperate <= limitsOfFan)
  {
    if (currentHumidity > goalHumid)
    {
      Serial.print("fanon");
      Serial.print("\n");
      fan.turnOn();
    }
    else
    {
      Serial.print("fanOff");
      Serial.print("\n");
      fan.turnOff();
    }
  }
  else
  {
    fanError = true;
  }

  controlHumidSeconds = millis()/ 1000;
};

void connectToUnoWifiWithMillisDelay(int delay)
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
        temp = Serial.readStringUntil('\n'); // Do not using char(13) instead '\n' 
        // Serial.println(temp);
        //  Wifi will be sent "perform/option=value_p\n"
        //? unoWifi Will send "current=" <-> Uno will send "current= ~~"
        //? unoWifi Will send "led=on"
        //? unoWifi Will send "switches=0,1,0"[aircon,fan,nutrient]
        //? uniWifi will send "switchesStatus="
        //? unoWifi Will send "setting=2.14,24,23" <-> Uno Will send "setting= ~~"

        char* perform = strtok((char*)temp.c_str(), "=");
        // Serial.println(perform);
        char* value_p = strtok(NULL,"");
        // char* value_p = strtok(NULL, "");

             if(strcmp(perform,CURRENT_KEY) == 0)
        {
          getCurrentValue();
          Serial.print(CURRENT_KEY);
          Serial.print("=");
          Serial.print(payload);
          Serial.print("\n");
        }
        else if(strcmp(perform,SWITCHES_STATUS_KEY) == 0)
        {
          String switchesStatusPayload = String(autoModeOnAircon) + "," + String(autoModeOnFan) + "," + String(autoModeOnNutrient);
          Serial.print(SWITCHES_KEY);
          Serial.print("=");
          Serial.print(switchesStatusPayload);
          Serial.print("\n");
        }

        else if(strcmp(perform,SWITCHES_KEY) == 0)
        {
          char *aircon_ptr = strtok(value_p, ",");
          autoModeOnAircon = atoi(aircon_ptr);
          char *fan_ptr = strtok(NULL, ",");
          autoModeOnFan = atoi(fan_ptr);
          char *nutrient_ptr = strtok(NULL, ",");
          autoModeOnNutrient = atoi(nutrient_ptr);

          if (!autoModeOnAircon) aircon.turnOff();
          if (!autoModeOnFan)    fan.turnOff();
          if (!autoModeOnNutrient)  nutrient.turnOff();
          
          String switchesPayload = String(autoModeOnAircon) + "," + String(autoModeOnFan) + "," + String(autoModeOnNutrient);

          Serial.print(SWITCHES_KEY);
          Serial.print("=");
          Serial.print(switchesPayload);
          Serial.print("\n");
        }

        else if(strcmp(perform,SETTING_KEY) == 0)
        {
          char* goalEC_ptr = strtok(value_p,",");
          goalEC = atof(goalEC_ptr);
          char* goalTemp_ptr = strtok(NULL,",");
          goalTemp = atoi(goalTemp_ptr);
          char* goalHumid_ptr = strtok(NULL,",");
          goalHumid = atoi(goalHumid_ptr);

          String settingPayload = "goalEC : "+ String(goalEC) + "," +  "goalTemp : " + String(goalTemp) + "," +  "goalHumid : "+ String(goalHumid);
          Serial.print(SETTING_KEY);
          Serial.print("=");
          Serial.print(settingPayload);
          Serial.print("\n");
        }
        else if (strcmp(perform, "led") == 0)
        {
          if (strcmp(value_p, "on") == 0)
          {
            led.turnOn();
            Serial.print("ledOn");
            Serial.print("\n");
          }
          else if (strcmp(value_p, "off") == 0)
          {
            led.turnOff();
            Serial.print("ledOff");
            Serial.print("\n");
          }
        }
        else if (strcmp(perform, SNAPSHOTS_KEY) == 0)
        {
          getCurrentValue();
          Serial.print(SNAPSHOTS_KEY);
          Serial.print("=");
          Serial.print(payload);
          Serial.print("\n");
        }
    }
}

#endif
