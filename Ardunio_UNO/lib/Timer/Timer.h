#ifndef Timer_h
#define Timer_h

#include <Arduino.h>

class Timer
{
private:
    
public:
   
   void Initialize();
   
   void AttatchPWM();
   void DetatchPWM();
};


extern Timer Timer1;

#endif