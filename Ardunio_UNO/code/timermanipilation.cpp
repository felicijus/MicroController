#include <Arduino.h>

// https://www.exp-tech.de/blog/arduino-tutorial-timer

/*
8Bit Timer0 nicht nutzen da für millis(), micros(), delay() genutzt
16Bit Timer1
8Bit Timer2
*/

//  f = Systemtakt[16000000 | 16MHz] / (prescaler * 256) [(optional) / i]

void setup() {
  Serial.begin(9600);

  TCCR2A = 0x00; // Wave Form Generation Mode 0: Normal Mode, OC2A disconnected
  TCCR2B = (1<<CS22) + (1<<CS21) + (0<<CS20); // prescaler = 1024 [every 1024th system clock count up]
  TIMSK2 = (1<<TOIE2); // interrupt when TCNT2 is overflowed

  //DDRB = 0b00000001;

  DDRB |= (1<<PB0);  // Portmanipulation: replaces pinMode(8, OUTPUT);
}

void loop() {
  /*PORTB |= (1<<PB0);
  //digitalWrite(8, HIGH);
  Serial.println("ON");
  

  delaynew(1000);

  PORTB &= ~(1<<PB0);
  //digitalWrite(8, LOW);
  Serial.println("OFF");

  delaynew(1000);
  */
}

ISR(TIMER2_OVF_vect){
  static int i = 0;
  i++;
  if(i==4){
    PORTB ^= (1<<PB0); // toggle PB0
    i=0;
  }
  
  
}