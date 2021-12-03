#ifndef SendIR_h
#define SendIR_h

#include <Arduino.h>

class SendIR
{
private:
   int _SEND_PIN; 
public:
   SendIR(int SEND_PIN);
   void sendNEC(unsigned long sigCode,unsigned int repeats=0, byte numBits=32);
   void mark(unsigned int mLen);
   void space(unsigned int sLen);

};

#endif