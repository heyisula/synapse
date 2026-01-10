#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

//#define DEBUG_MODE //Commenting to turn off debug mode


#ifdef DEBUG_MODE
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.println(x)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
#endif

#endif
