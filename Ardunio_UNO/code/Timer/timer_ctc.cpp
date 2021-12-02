#include <Arduino.h>

// https://www.exp-tech.de/blog/arduino-tutorial-timer
// https://wolles-elektronikkiste.de/timer-und-pwm-teil-1
// https://eleccelerator.com/avr-timer-calculator/ - Rechner

/*
8Bit Timer0 nicht nutzen da für millis(), micros(), delay() genutzt
16Bit Timer1
8Bit Timer2
*/

//  f = Systemtakt / ((OCRnx) * prescaler)
// 100000 Hz = (16000000Hz /((4+1)*32))

volatile unsigned long timer2_micros = 0;


void Initialize()
{
  cli();

  //RESET ALL TIMER2 REGISTER
  TCCR2A = 0x00;
  TCCR2B = 0x00;
  TIMSK2 = 0x00;
  TCNT2 = 0x00;
  OCR2A = 0x00;
  OCR2B = 0x00;


  // SET TIMER2
  //CTC - Clear Timer on Compare Match 
  TCCR2A = (1<<WGM21) + (0<<WGM20);  
  TCCR2B = (0<<WGM22);

  //TCCR2B – Timer/Counter Control Register B
  TCCR2B = (0<<CS22) + (1<<CS21) + (1<<CS20); 
  // 0 1 1  clk/32 (From prescaler)
  
  //OCR2A – Output Compare Register A
  OCR2A = 4;

  sei();

  Serial.println("Timer SET");
}

void AttatchInterrupt(){
  //TIMSK2 – Timer/Counter2 Interrupt Mask Register
  TIMSK2 |= (1<<OCIE2A);   //AttachInterrupt when Compare Match with OCR2A
}

void DetatchInterrupt(){
  //TIMSK2 – Timer/Counter2 Interrupt Mask Register
  TIMSK2 &= ~(1<<OCIE2A); //DetatchInterrupt when Compare Match with OCR2A
}

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
  Initialize();
  AttatchInterrupt();
  
} 
void loop() { 
 
  delay(1000);
  //delay_micros_10(100000); //100k us = 1000ms

  //Serial.println(micros());
  Serial.println(micros_10());
}


