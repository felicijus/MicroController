#include <Arduino.h>

// https://www.exp-tech.de/blog/arduino-tutorial-timer
// https://wolles-elektronikkiste.de/timer-und-pwm-teil-1
// https://eleccelerator.com/avr-timer-calculator/ - Rechner

/*
8Bit Timer0 nicht nutzen da für millis(), micros(), delay() genutzt
16Bit Timer1
8Bit Timer2
*/

//  f = Systemtakt[16000000 | 16MHz] / (counterstart * 256) [(optional) / i]

byte counterstart = 64;
unsigned int scaleFactor = 1;

// the counterstart is set so that timer0 ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.


volatile unsigned long timer2_overflow_count = 0;


ISR(TIMER2_OVF_vect)
{
	++timer2_overflow_count;
}


unsigned long micros_own(){

  unsigned long m;
	//uint8_t oldSREG = _MMIO_BYTE(0x100), t;
	uint8_t oldSREG = SREG, t;

	cli();
	m = timer2_overflow_count;
	t = TCNT2;

	if ((TIFR2 & 1<<(TOV2)) && (t < 255)){
    m++;
  }
	SREG = oldSREG;
	
	//return ((m << 8) + t) * (4);   //16 clock cycles per us    64 / clockCyclesPerMicrosecond() --> 4 clock cycles
  return ((timer2_overflow_count << 8) * 4);
}



void setup() {
  Serial.begin(115200);

  cli();
  // RESET ALL TIMER REGISTER
  TCCR2A = 0x00;
  TCCR2B = 0x00;
  TIMSK2 = 0x00;
  TCNT2 = 0x00;
  

  // Set TIMER REGISTER
  TCCR2A = 0x00; // Wave Form Generation Mode 0: Normal Mode, OC2A disconnected
  TCCR2B = (1<<CS22) + (0<<CS21) + (0<<CS20); // counterstart = 64 [every 8th system clock count up]
  TIMSK2 = (1<<TOIE2); // interrupt when TCNT2 is overflowed
  //TCNT2 = counterstart; //count from "counterstart" to 256
  sei();

  //_MMIO_BYTE(0x0100) = 0b0;  //SRAM first Adress
}


void loop() {
  delay(1000);

  Serial.println(micros());
  

  Serial.println(micros_own());
  
  
  //Serial.println(_MMIO_BYTE(0x0101));
}