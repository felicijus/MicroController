#ifndef Timer_h
#define Timer_h

#include <Arduino.h>

class Timer
{
private:
    
public:
   
   void Initialize();
   
   void activatePWM();
   void disablePWM();
};


extern Timer Timer1;

#endif