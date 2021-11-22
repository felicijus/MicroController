#ifndef SendIR_h
#define SendIR_h

#include <Arduino.h>

class SendIR
{
private:
   int _pin; 
public:
   SendIR(int pin);
   void sendHexNEC(unsigned long sigCode, byte numBits, unsigned char repeats);
   void mark(unsigned int mLen);
   void space(unsigned int sLen);

};

#endif