#include <Arduino.h>
#include "SendIR.h"


unsigned char periodHigh = 0; //calculated once for each signal sent in initSoftPWM
unsigned char periodLow = 0; //calculated once for each signal sent in initSoftPWM

unsigned long sigTime = 0; //used in mark & space functions to keep track of time
unsigned long sigStart = 0; //used to calculate correct length of existing signal, to handle some repeats


//#define NEC_HEX_VALUE 0x20DF22DDUL //UL makes this an unsigned long
//#define NEC_BIT_COUNT 32

SendIR::SendIR(int pin)
{
    pinMode(pin, OUTPUT);
    _pin=pin;
}

void SendIR::sendHexNEC(unsigned long sigCode, byte numBits, unsigned char repeats) {
  /*  A basic 32 bit NEC signal is made up of:
   *  1 x 9000 uSec Header Mark, followed by
   *  1 x 4500 uSec Header Space, followed by
   *  32 x bits uSec ( 1- bit 560 uSec Mark followed by 1690 uSec space; 0 - bit 560 uSec Mark follwed by 560 uSec Space)
   *  1 x 560 uSec Trailer Mark
   *  There can also be a generic repeat signal, which is usually not neccessary & can be replaced by sending multiple signals
   */
#define NEC_HEADER_MARK 9000
#define NEC_HEADER_SPACE 4500
#define NEC_ONE_MARK 560
#define NEC_ZERO_MARK 560
#define NEC_ONE_SPACE 1690
#define NEC_ZERO_SPACE 560
#define NEC_TRAILER_MARK 560

  unsigned long bitMask = (unsigned long) 1 << (numBits - 1); //allows for signal from 1 bit up to 32 bits
  

  sigTime = micros(); //keeps rolling track of signal time to avoid impact of loop & code execution delays
  sigStart = sigTime; //remember for calculating first repeat gap (space), must end 108ms after signal starts
  // First send header Mark & Space
  mark(NEC_HEADER_MARK);
  space(NEC_HEADER_SPACE);

  while (bitMask) {
    if (bitMask & sigCode) { //its a One bit
      mark(NEC_ONE_MARK);
      space(NEC_ONE_SPACE);
    }
    else { // its a Zero bit
      mark(NEC_ZERO_MARK);
      space(NEC_ZERO_SPACE);
    }
    bitMask = (unsigned long) bitMask >> 1; // shift the mask bit along until it reaches zero & we exit the while loop
  }
  // Last send NEC Trailer MArk
  mark(NEC_TRAILER_MARK);

  //now send the requested number of NEC repeat signals. Repeats can be useful for certain functions like Vol+, Vol- etc
  /*  A repeat signal consists of
   *   A space which ends 108ms after the start of the last signal in this sequence
  *  1 x 9000 uSec Repeat Header Mark, followed by
  *  1 x 2250 uSec Repeat Header Space, followed by
  *  32 x bits uSec ( 1- bit 560 uSec Mark followed by 1690 uSec space; 0 - bit 560 uSec Mark follwed by 560 uSec Space)
  *  1 x 560 uSec repeat Trailer Mark
  */
  return;
}


void SendIR::mark(unsigned int mLen) { //uses sigTime as end parameter
  sigTime += mLen; //mark ends at new sigTime
  unsigned long now = micros();
  unsigned long dur = sigTime - now; //allows for rolling time adjustment due to code execution delays
  if (dur == 0) return;
  while ((micros() - now) < dur) { //just wait here until time is up
    digitalWrite(_pin, HIGH);
    if (periodHigh) delayMicroseconds(periodHigh);
    digitalWrite(_pin, LOW);
    if (periodLow)  delayMicroseconds(periodLow);
  }
}

void SendIR::space(unsigned int sLen) { //uses sigTime as end parameter
  sigTime += sLen; //space ends at new sigTime
  unsigned long now = micros();
  unsigned long dur = sigTime - now; //allows for rolling time adjustment due to code execution delays
  if (dur == 0) return;
  while ((micros() - now) < dur) ; //just wait here until time is up
}