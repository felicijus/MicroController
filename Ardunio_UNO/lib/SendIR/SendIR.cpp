#include <Arduino.h>
#include <SendIR.h>

#include <Timer.h>


// https://techdocs.altium.com/display/FPGA/NEC+Infrared+Transmission+Protocol


uint32_t time = 0;  // used in mark & space functions to keep track of time
uint32_t start = 0; // used to calculate correct length of existing signal, and  handle repeats

SendIR::SendIR(int SEND_PIN)
{
  pinMode(SEND_PIN, OUTPUT);
  _SEND_PIN = SEND_PIN;
}

void SendIR::sendNEC(unsigned long NEC_Code, unsigned int repeats, byte numBits)
{
  // PWM Initialize
  Timer1.Initialize();
  Timer1.disablePWM();


  // NEC Initial Message:
  //   1 x 9000 us Header Mark, followed by
  //   1 x 4500 us Header Space, followed by
  //   32 Bits ->
  //               1 - 562.5 us  Mark followed by 1690 us Space;
  //               0 - 562.5 us  Mark follwed by 562.5 us  us Space)
  //   1 x 562.5 us  Trailer Mark

  //  Possible generic repeat signal

#define NEC_UNIT 562 // normally 562.5 us

#define NEC_HEADER_MARK           (16 * NEC_UNIT) // 8.992 us -> 9000 us
#define NEC_HEADER_SPACE          (8 * NEC_UNIT) // 4.496 us -> 4500 us

#define NEC_BIT_MARK              NEC_UNIT        
#define NEC_ONE_SPACE             (3 * NEC_UNIT) // 1686 us -> 1687.5 us
#define NEC_ZERO_SPACE            NEC_UNIT      
#define NEC_TRAILER_MARK          NEC_UNIT    

#define NEC_REPEAT_HEADER_SPACE   (4 * NEC_UNIT) // 2248 us -> 2250

  
  time = micros(); // Tracking of Signal Time
  start = time;    // Signal Start Time
  
  
  // Start NEC Initial Message:

  // Header
  mark(NEC_HEADER_MARK);
  space(NEC_HEADER_SPACE);

  // 32 Bits
  for (uint8_t bit = 0; bit < numBits; bit++) // signal from 1 bit up to 32 bits using LSB
  {
    if (NEC_Code & 1)
    { // 1 bit
      mark(NEC_BIT_MARK);
      space(NEC_ONE_SPACE);
    }
    else
    { // 0 bit
      mark(NEC_BIT_MARK);
      space(NEC_ZERO_SPACE);
    }
    NEC_Code >>= 1;
  }
  
  // Trailing Mark
  mark(NEC_TRAILER_MARK);

  
  


  // NEC Repeat Signal:
  //   1 x 9000 us Header Mark, followed by
  //   1 x 2250 us Header Space, followed by
  //   1 x 562.5 us Trailing Mark
  
  // Start NEC Repeat Signal:

  if (repeats == 0) return; //no Repeat Codes
     
  else if (repeats > 0)
  {                                 
    space(108000 - (time - start)); // first Repeat Code start 108ms after Initial Message -> space(x) -> x remaining time until 108 ms after start is reaced
    mark(NEC_HEADER_MARK);
    space(NEC_REPEAT_HEADER_SPACE);
    mark(NEC_TRAILER_MARK);
  }

  while (--repeats > 0)
  {                                                                            
    space(108000 - NEC_HEADER_MARK - NEC_REPEAT_HEADER_SPACE - NEC_TRAILER_MARK); // next Repeat Code start 108ms after previous Repeat Code
    mark(NEC_HEADER_MARK);
    space(NEC_REPEAT_HEADER_SPACE);
    mark(NEC_TRAILER_MARK);
  }

  return;
}

void SendIR::mark(unsigned int markLength)  // 1 Bit
{                     
  time += markLength; // update time to end of Mark

  uint32_t now = micros();  // current Time -> now
  uint32_t dur = time - now; // set dur-ration with now

  Timer1.activatePWM();

  if (dur == 0) return;

  while ((micros() - now) < dur); // end Mark when dur-ration is over

  Timer1.disablePWM();
}

void SendIR::space(unsigned int spaceLength)  // 0 Bit
{                      
  time += spaceLength;  // update Time to end of Space

  uint32_t now = micros();
  uint32_t dur = time - now;

  if (dur == 0) return;
  
  while ((micros() - now) < dur); // end Space when dur-ration is over
}