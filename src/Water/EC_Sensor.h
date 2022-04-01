#ifndef EC_SENSOR_H
#define EC_SENSOR_H
#include <SoftwareSerial.h> //we have to include the SoftwareSerial library, or else we can't use it
/*
  EC 측정 일지
  수돗물일 경우 기본 ec값은 0.2로 측정되었다
  용액 A와 용액 B 400배 희석인 경우
  반복측정 결과
  2.05이 지속적으로 관찰됨

  근거로 바운더리 추산
*/

enum EC_RESULT
{
    EC_RESULT_OK,
    EC_RESULT_ERR_TIMEOUT
};

class ECSensor
{
private:
    String sensorstring = "";            // a string to hold the data from the Atlas Scientific product
    SoftwareSerial myserial;

public:
    ECSensor(int rxInput, int txInput) : myserial(rxInput,txInput) {}


    void begin()
    {
        myserial.begin(9600);     // set baud rate for the software serial port to 9600
        sensorstring.reserve(30); // set aside some bytes for receiving data from Atlas Scientific product
       
        /* set EC calibration Export */
        Serial.println("EC Import Start");
        myserial.print("Import,FDD44200803F\r");
        myserial.print("Import,312E30300000\r");
        myserial.print("Import,3D0A3F128AFE\r");
        myserial.print("Import,FFF0C0FAFF28\r");
        myserial.print("Import,1B0500B00E1F\r");
        myserial.print("Import,00409C4780F1\r");
        myserial.print("Import,1200F0FA1700\r");
        myserial.print("Import,404946010102\r");
        myserial.print("Import,0100000000B9\r");
        myserial.print("Import,56\r");
        if(myserial.readStringUntil(13) == "*DONE"){    
            Serial.println("Import Success");
        }
        myserial.readString();    // set sensorString Clear;

   
    }

    float getEC()
    {
        myserial.listen();
        float ecValue = -1;
        unsigned long currentMillis = millis();
        bool sensor_string_complete = false; // have we received all the data from the Atlas Scientific product

        while (!sensor_string_complete)
        {
            if (millis() > currentMillis + 1000)     // if Data Requset fail in 1 sec, Stop and Return -1;
            { 
                return ecValue;
            }
            if (myserial.available() > 0)
            {                                        // if we see that the Atlas Scientific product has sent a character
                char inchar = (char)myserial.read(); // get the char we just received
                sensorstring += inchar;              // add the char to the var called sensorstring
                if (inchar == '\r')
                {                                  // if the incoming character is a <CR>
                    sensor_string_complete = true; // set the flag
                }
            }
            if (sensor_string_complete == true)
            { // if a string from the Atlas Scientific product has been received in its entirety
                if (isdigit(sensorstring[0]) == false)
                {                                 // if the first character in the string is a digit
                    Serial.println(sensorstring); // send that string to the PC's serial monitor
                }
                else // if the first character in the string is NOT a digit
                {
                    ecValue = getFloat(); // then call this function
                }                         // reset the flag used to tell if we have received a completed string from the Atlas Scientific product
            }
        }
        sensorstring = ""; // clear the string
        sensor_string_complete = false;
        ecValue = ecValue / 1000;
        return ecValue;
    }

    float getFloat(void)
    { // this function will pars the string

        char sensorstring_array[30]; // we make a char array
        char *EC;                    // char pointer used in string parsing
        // char *TDS;                                          //char pointer used in string parsing
        // char *SAL;                                          //char pointer used in string parsing
        // char *GRAV;                                         //char pointer used in string parsing
        float f_ec; // used to hold a floating point number that is the EC

        sensorstring.toCharArray(sensorstring_array, 30); // convert the string to a char array
        EC = strtok(sensorstring_array, ",");             // let's pars the array at each comma
        // Serial.print("EC:");                                //we now print each value we parsed separately
        // Serial.println(EC);
        f_ec = atof(EC); // uncomment this line to convert the char to a float
        return f_ec;
    }
};

#endif