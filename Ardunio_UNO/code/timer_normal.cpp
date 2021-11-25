#include <Arduino.h>

// https://www.exp-tech.de/blog/arduino-tutorial-timer
// https://wolles-elektronikkiste.de/timer-und-pwm-teil-1
// https://eleccelerator.com/avr-timer-calculator/ - Rechner

/*
8Bit Timer0 nicht nutzen da für millis(), micros(), delay() genutzt
16Bit Timer1
8Bit Timer2
*/

//  f = Systemtakt[16000000 | 16MHz] / (prescaler * 256) [(optional) / i]


byte counterStart = 236;
unsigned int scaleFactor = 1;

volatile unsigned long timer2_micros = 0;

ISR(TIMER2_OVF_vect){
  timer2_micros++;
}

unsigned long micros_own(){
  return timer2_micros;
}

void setup() {
  Serial.begin(115200);

   // RESET ALL TIMER REGISTER
  TCCR2A = 0x00;
  TCCR2B = 0x00;
  TIMSK2 = 0x00;
  TCNT2 = 0x00;

  TCCR2A = 0x00; // Wave Form Generation Mode 0: Normal Mode, OC2A disconnected
  TCCR2B = (0<<CS22) + (1<<CS21) + (0<<CS20); // prescaler = 8 [every 8th system clock count up]
  TIMSK2 = (1<<TOIE2); // interrupt when TCNT2 is overflowed
  TCNT2 = counterStart; //count from "counterstart" to 256

}

void loop() {
  
}







