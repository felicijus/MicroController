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
unsigned int scaleFactor = 1;


ISR (TIMER2_COMPA_vect){  // Interrupt Service Routine 
  static unsigned int counter = 0;
  counter++;
  if(counter==scaleFactor){
    timer2_micros++; 
    counter = 0; 
  }
}

unsigned long own_micros(){
  return timer2_micros;
}

void setup(){ 
  Serial.begin(9600);

  cli();
  TCCR2A = (1<<WGM21); // Wave Form Generation Mode 2: CTC, OC2A disconnected
  TCCR2B = (0<<CS22) + (1<<CS21) + (0<<CS20); // prescaler = 256
  TIMSK2 = (1<<OCIE2A); // interrupt when Compare Match with OCR2A
  OCR2A = 19;

  Serial.println("Timer SET");
  sei();
  
} 
void loop() { 
 // do something else
  delay(1000);
  //Serial.println(timer2_micros);
  Serial.println(own_micros());
}
