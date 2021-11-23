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

void setup() {
  Serial.begin(9600);

  TCCR2A = 0x00; // Wave Form Generation Mode 0: Normal Mode, OC2A disconnected
  TCCR2B = (0<<CS22) + (1<<CS21) + (0<<CS20); // prescaler = 8 [every 8th system clock count up]
  TIMSK2 = (1<<TOIE2); // interrupt when TCNT2 is overflowed
  TCNT2 = counterStart; //count from "counterstart" to 256

  //DDRB = 0b00000001;

  DDRB |= (1<<PB0);  // Portmanipulation: replaces pinMode(8, OUTPUT);
}

void loop() {
  
}


ISR(TIMER2_OVF_vect){
  static int i = 0;
  TCNT2 = counterStart; //couterStart = 1
  i++;
  if(i==scaleFactor){
    PORTB ^= (1<<PB0);
    
    i=0;
  }
}




