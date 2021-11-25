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


volatile unsigned long timer2_micros = 0;

ISR (TIMER2_COMPA_vect){  // Interrupt Service Routine 
  timer2_micros++;
}

unsigned long micros_10(){
  return timer2_micros;
}

void delay_micros_10(unsigned long us_10)
{
	uint32_t start = micros_10();

	while (us_10 > 0) {
		yield();
		while ( us_10 > 0 && (micros_10() - start) >= 1) {
			us_10--;
			start += 1;
    }
	}
}

void setup(){ 
  Serial.begin(115200);

  cli();
  TCCR2A = (1<<WGM21); // Wave Form Generation Mode 2: CTC, OC2A disconnected
  TCCR2B = (0<<CS22) + (0<<CS21) + (1<<CS20); // prescaler = 8
  TIMSK2 = (1<<OCIE2A); // interrupt when Compare Match with OCR2A
  OCR2A = 159; //normally 16o but more accurate

  sei();

  Serial.println("Timer SET");
} 
void loop() { 
 
  delay(1000);
  //delay_micros_10(100000); //100k us = 1000ms

  //Serial.println(micros());
  Serial.println(micros_10());
}
