#ifndef LIVMEGA_H
#define LIVMEGA_H

#include <BH1750.h>
#include <Adafruit_HTU21DF.h>
#include "Controller/controller.h"
#include "Water/EC_Sensor.h"
#include "Water/PH_Sensor.h"
#include "Water/WL_Sensor.h"
#include "Water/WT_Sensor.h"
#include "Circumstacne/co2_sensor.h"

/* Define pin Number */ 


#define WATER_LOW_PIN             13   // DFROBOT SEN0204
#define WATER_HIGH_PIN            12   // DFROBOT SEN0204
//                                11   // None
//                                10   // None
#define NUTRIENT_RELAY_PIN        A0   //  12 V
#define LED_RELAY_PIN              9   // LED      220 V 
#define AIRCON_FAN_RELAY_PIN       8   //      12 V
#define AIRCON_RELAY_PIN           7   // Nutrient 220 V
#define WATER_TEMP_PIN             6   // YWROBOT DS18B20 + SEN050007
#define EC_TX_PIN                  5   // Atlas EC Sensor
#define EC_RX_PIN                  4   // 
#define CO2_TX_PIN                 3   // DFROBOT MHZ19 
#define CO2_RX_PIN                 2   // 
#define PH_SENSOR_PIN             A2   // DFROBOT PH Sensor

/*Temp,Humid and Lux sensors are I2C Communicate*/

Controller fan, led, aircon, nutrient; 

ECSensor ecSensor(EC_RX_PIN,EC_TX_PIN);
PHSensor phSensor(PH_SENSOR_PIN);
WTSensor waterTempSensor(WATER_TEMP_PIN);
WLSensor waterLevelSensor(WATER_HIGH_PIN,WATER_LOW_PIN);
Adafruit_HTU21DF temphumidSensor;
BH1750 luxSensor(0x23);
co2Sensor ppmSensor(CO2_RX_PIN,CO2_TX_PIN);


// Timing
extern volatile unsigned long timer0_millis;       // Reset Time Value

unsigned long currentSeconds;                      // Snapshot of current time
unsigned long controlECSeconds;                    // Timer to check EC and control Nutrient Pump
unsigned long controlTempSeconds;                  // Timer to check Temperature and control Aircon
unsigned long controlHumidSeconds;                 // Timer to check Temperature and control Aircon

unsigned long controlECPeriod    =   30*60;           // Time in Minutes between controling Electronic Conductivity
unsigned long controlTempPeriod  =    5*60;           // Time in Minutes between controling Temperature
unsigned long controlHumidPeriod =    5*60;           // Time in Minutes between controling Humiditiy

bool isTurnOnAircon = false;
bool isTurnOnFan = false;
bool isTurnOnNutrient = false;

unsigned long wifiWaitPeriod = 5000;               // Time to wait for sensor data if message.available


float currentTemp = -1;
float currentHumidity = -1;
float currentLux = -1;
float currentPPM = -1;
float currentWaterTemp = -1;
water_level currentWaterLevel = WATER_LEVEL_ERROR;
float currentEC = -1;
float currentPH = -1;

float goalEC = -1;
float goalTemp = -1;
float goalHumid = -1;
unsigned long turnOnLEDPeriod   = 18 * 60;
unsigned long turnOffLEDPeriod  =  6 * 60;


String payload = String(currentTemp) + "," + String(currentHumidity)+ "," + String(currentLux)+ "," + String(currentPPM) + "," +  String(currentWaterTemp) + "," + currentWaterLevel  + "," + String(currentPH) + "," + String(currentEC);


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
  currentTemp = temphumidSensor.readTemperature();           
  currentHumidity = temphumidSensor.readHumidity();          
  currentLux = luxSensor.readLightLevel();;                  
  currentPPM = ppmSensor.getPPM();                           
  currentWaterTemp = waterTempSensor.getWaterTemperature();  
  currentWaterLevel = waterLevelSensor.getWaterLevel_enum(); 
  currentEC = ecSensor.getEC();                              
  currentPH = phSensor.getPHAvg();                           

  payload = String(currentTemp) + "," + String(currentHumidity)+ "," + String(currentLux)+ "," + String(currentPPM) + "," +  String(currentWaterTemp) + "," + String(currentWaterLevel)  + "," + String(currentPH) + "," + String(currentEC);

};


void controlEC()
{
  // Serial.print(currentSeconds);
  // Serial.println("Control EC Start");
  currentEC = constrain(ecSensor.getEC(),0,10);
  if (currentEC < goalEC && waterLevelSensor.getWaterLevel_enum() > WATER_LEVEL_LOW)
  {
    float difference = currentEC - goalEC;
    nutrient.turnOn();
    connectToESPWithMillisDelay(1000 * difference);
    nutrient.turnOff();
  }

  controlECSeconds = millis() /  1000;
};

void controlTemp()
{
  // Serial.print(currentSeconds);
  // Serial.println("Control Temp Start");
  currentTemp = temphumidSensor.readTemperature();
  if (currentTemp > goalTemp) aircon.turnOn();  
  else                        aircon.turnOff();
  controlTempSeconds = millis() / 1000;
};

void controlHumid()
{ 
  // Serial.print(currentSeconds);
  // Serial.println("Control Humid Start");
  currentHumidity = temphumidSensor.readHumidity();
  if(currentHumidity > goalHumid) fan.turnOn();
  else                            fan.turnOff();
  controlHumidSeconds = millis()/ 1000;
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
        temp = Serial.readStringUntil('\n'); // Do not using char(13) instead '\n' 
        // Serial.println(temp);
        //  Wifi will be sent "perform/option=value_p\n"
        char* perform = strtok((char*)temp.c_str(), "/");
        char* option = strtok(NULL,"=");
        char* value_p = strtok(NULL, "");

             if(strcmp(perform,"current") == 0)
        {
          takeCurrentValue();
          Serial.print(payload);
          Serial.print("\n");
        }

        else if(strcmp(perform,"controller") == 0)
        {
               if (strcmp(option, "reset")   == 0)                                 asm volatile("jmp 0");
          else if (strcmp(option, "led")     == 0 && strcmp(value_p,"on")  == 0)   led.turnOn();
          else if (strcmp(option, "led")     == 0 && strcmp(value_p,"off") == 0)   led.turnOff();    
          else if (strcmp(option, "aircon")  == 0 && strcmp(value_p,"on")  == 0)   isTurnOnAircon = true;
          else if (strcmp(option, "aircon")  == 0 && strcmp(value_p,"off") == 0)   isTurnOnAircon = false;
          else if (strcmp(option, "fan")     == 0 && strcmp(value_p,"on")  == 0)   isTurnOnFan = true;
          else if (strcmp(option, "fan")     == 0 && strcmp(value_p,"off") == 0)   isTurnOnFan = false;
          else if (strcmp(option, "nutrient")== 0 && strcmp(value_p,"on")  == 0)   isTurnOnNutrient = true;
          else if (strcmp(option, "nutrient")== 0 && strcmp(value_p,"off") == 0)   isTurnOnNutrient = false;
          // else if(strcmp(option,"abjustEC") == 0)           controlEC();
          // else if(strcmp(option,"abjustHumid") == 0)        controlHumid();
        }

        else if(strcmp(perform,"setting") == 0)
        {
               if(strcmp(option,"goalEC") == 0)             goalEC = atof(value_p);
          else if(strcmp(option,"goalTemp") == 0)           goalTemp = atof(value_p);
          else if(strcmp(option,"goalHumid") == 0)          goalHumid = atof(value_p);
          // else if(strcmp(option,"controlECPeriod") == 0 )   controlECPeriod = atof(value_p);
          // else if(strcmp(option,"controlTempPeriod") == 0 ) controlTempPeriod = atof(value_p);
          // else if(strcmp(option,"controlHumidPeriod") == 0 )controlHumidPeriod = atof(value_p);
          // else if(strcmp(option,"controlLedPeriod") == 0 )  controlLedPeriod = atof(value_p);
          // else if(strcmp(option,"turnOnLEDPeriod") == 0 )   turnOnLEDPeriod = atof(value_p);
          // else if(strcmp(option,"turnOffLEDPeriod") == 0 )  turnOffLEDPeriod = atof(value_p);

        }   
    }
}

#endif
