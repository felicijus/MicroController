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


// the counterstart is set so that timer0 ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_TIMER2_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER2_OVERFLOW / 1000)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER2_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

volatile unsigned long timer2_overflow_count = 0;
volatile unsigned long timer2_millis = 0;
static unsigned char timer2_fract = 0;


ISR(TIMER2_OVF_vect)
{
	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	unsigned long m = timer2_millis;
	unsigned char f = timer2_fract;

	m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX) {
		f -= FRACT_MAX;
		m += 1;
	}

	timer2_fract = f;
	timer2_millis = m;
	timer2_overflow_count++;
}

unsigned long millis_own()
{
	unsigned long m;
	uint8_t oldSREG = SREG;

	// disable interrupts while we read timer0_millis or we might get an
	// inconsistent value (e.g. in the middle of a write to timer0_millis)
	cli();
	m = timer2_millis;
	SREG = oldSREG;

	return m;
}

unsigned long micros_own(){
  //return timer2_overflow_count;
  
  unsigned long m;
	//uint8_t oldSREG = _MMIO_BYTE(0x100), t;
	uint8_t oldSREG = SREG, t;


	cli();
	m = timer2_overflow_count;
	t = TCNT2; //Maybe

	if ((TIFR2 & _BV(TOV2)) && (t < 255)) //Maybe
		m++;


	SREG = oldSREG;
	
	return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond()); //Maybe
}

byte counterstart = 64;
unsigned int scaleFactor = 1;

void setup() {
  Serial.begin(9600);

  cli();
  // RESET ALL REGISTER
  /*TCCR2A = 0x00;
  TCCR2B = 0x00;
  TIMSK2 = 0x00;
  TCNT2 = 0x00;
  */

  // Set REGISTER
  TCCR2A = 0x00; // Wave Form Generation Mode 0: Normal Mode, OC2A disconnected
  TCCR2B = (1<<CS22) + (0<<CS21) + (0<<CS20); // counterstart = 64 [every 8th system clock count up]

  TIMSK2 = (1<<TOIE2); // interrupt when TCNT2 is overflowed
  
  TCNT2 = counterstart; //count from "counterstart" to 256
  sei();

  //_MMIO_BYTE(0x0100) = 0b0;  //SRAM first Adress
}


void loop() {
  
  Serial.println(micros());
  //Serial.println(millis());

  Serial.println(micros_own());
  //Serial.println(millis_own());
  
  //Serial.println(_MMIO_BYTE(0x0101));
  
  
}