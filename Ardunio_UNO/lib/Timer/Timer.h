#ifndef Timer_h
#define Timer_h

#include <Arduino.h>

class Timer
{
private:
    
public:
   
   void Initialize();
   void AttatchInterrupt();
   void DetatchInterrupt();

   unsigned long micros_10();

};


extern Timer Timer2;

#endif