#include "changeLengthSetLCD.h"

String changeLengthSetLCD(String message,int width)
{ 
  int iMax = width - message.length();
  for(int i = 0; i< iMax; ++i)
  {
    message = " "+message;
  }
  return message;
};

String changeLengthSetLCD(float value,int width)
{ 
  String message = String(value);
  return changeLengthSetLCD(message,width);
};