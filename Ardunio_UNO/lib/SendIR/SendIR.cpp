#include <Arduino.h>
#include <SendIR.h>

#include <Timer.h>


unsigned char carrierFreq = 38; //NEC Carrier Frequenzie
unsigned char period = 0; //calculated once for each signal sent in initSoftPWM
unsigned char periodHigh = 0; 
unsigned char periodLow = 0; 

uint32_t time = 0;  //used in mark & space functions to keep track of time
uint32_t start = 0; //used to calculate correct length of existing signal, to handle some repeats



SendIR::SendIR(int SEND_PIN)
{
  pinMode(SEND_PIN, OUTPUT);
  _SEND_PIN = SEND_PIN;


  //PWM Initialize
  period = 1000/carrierFreq;  //38 kHz in us -> 26.32
  periodHigh = (period + 1) / 3; //33% Duty-Cycle -> + 1 round off
  periodLow = period - periodHigh;
    
  periodHigh -= 6; //Trim it based on measurementt from Oscilloscope
  periodLow  -= 11; //Trim it based on measurementt from Oscilloscope
}

void SendIR::sendNEC(unsigned long NEC_Code, unsigned int repeats, byte numBits)
{
  Timer1.Initialize();
  Timer1.DetatchPWM();

  
  /*  A basic 32 bit NEC signal is made up of:
   *  1 x 9000 uSec Header Mark, followed by
   *  1 x 4500 uSec Header Space, followed by
   *  32 x bits uSec ( 1- bit 560 uSec Mark followed by 1690 uSec space; 0 - bit 560 uSec Mark follwed by 560 uSec Space)
   *  1 x 560 uSec Trailer Mark
   *  There can also be a generic repeat signal, which is usually not neccessary & can be replaced by sending multiple signals
   */

#define NEC_UNIT 560 // 21.28 periods of 38 kHz <- 560 / 26.32

#define NEC_HEADER_MARK   (16 * NEC_UNIT) // 9000 -> 8960
#define NEC_HEADER_SPACE  (8 * NEC_UNIT) // 4500 -> 4480

#define NEC_BIT_MARK      NEC_UNIT  // 560
#define NEC_ONE_SPACE     (3 * NEC_UNIT) //1690 -> 1680 
#define NEC_ZERO_SPACE    NEC_UNIT  // 560
#define NEC_TRAILER_MARK  NEC_UNIT  // 560

#define NEC_REPEAT_HEADER_SPACE   (4 * NEC_UNIT) // 2250 -> 2240



  time = micros();     //keeps rolling track of signal time to avoid impact of loop & code execution delays
  start = time; //remember for calculating first repeat gap (space), must end 108ms after signal starts

  // First send header Mark & Space
  mark(NEC_HEADER_MARK);
  space(NEC_HEADER_SPACE);

  for (uint8_t bit = 0; bit < numBits; bit++)
  { //allows for signal from 1 bit up to 32 bits LSB
    if (NEC_Code & 1)
    { //its a One bit
      mark(NEC_BIT_MARK);
      space(NEC_ONE_SPACE);
    }
    else
    { // its a Zero bit
      mark(NEC_BIT_MARK);
      space(NEC_ZERO_SPACE);
    }
    NEC_Code >>= 1;
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
  //First calcualte length of space for first repeat
  //by getting length of signal to date and subtracting from 108ms

  if (repeats == 0)
    return; //finished - no repeats
  else if (repeats > 0)
  {                                               //first repeat must start 108ms after first signal
    space(108000 - (time - start)); //first repeat Header should start 108ms after first signal
    mark(NEC_HEADER_MARK);
    space(NEC_HEADER_SPACE / 2); //half the length for repeats
    mark(NEC_TRAILER_MARK);
  }

  while (--repeats > 0)
  {                                                                            //now send any remaining repeats
    space(108000 - NEC_HEADER_MARK - NEC_HEADER_SPACE / 2 - NEC_TRAILER_MARK); //subsequent repeat Header must start 108ms after previous repeat signal
    mark(NEC_HEADER_MARK);
    space(NEC_HEADER_SPACE / 2); //half the length for repeats
    mark(NEC_TRAILER_MARK);
  }

  return;
}

void SendIR::mark(unsigned int markLength)
{                      //uses time as end parameter
  time += markLength; //mark ends at new time

  uint32_t now = micros();
  uint32_t dur = time - now; //allows for rolling time adjustment due to code execution delays

  Timer1.AttatchPWM();
  if (dur == 0)return;
  
  
  
  
  
  while ((micros() - now) < dur) 
  { //Software PWM normally 38kHz 33% Duty-Cycle


    
    /*digitalWrite(_SEND_PIN, HIGH);
    if (periodHigh) delayMicroseconds(periodHigh);
    digitalWrite(_SEND_PIN, LOW);
    if (periodLow)  delayMicroseconds(periodLow);*/
  }
  Timer1.DetatchPWM();
}

void SendIR::space(unsigned int spaceLength)
{                      //uses time as end parameter
  time += spaceLength; //space ends at new time

  uint32_t now = micros();
  uint32_t dur = time - now; //allows for rolling time adjustment due to code execution delays

  if (dur == 0) return;
  while ((micros() - now) < dur); //ZERO
}