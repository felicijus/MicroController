#include <Arduino.h>


//OC1B -> PB2 -> PIN10



void setup(){ 
  // Clear OC1A and OC1B on Compare Match / Set OC1A and OC1B at Bottom; 
  // Wave Form Generator: Fast PWM 14, Top = ICR1

  TCCR1A = (1<<COM1A1) + (1<<COM1B1) + (1<<WGM11); 
  TCCR1B = (1<<WGM13) + (1<<WGM12);
  TCCR1B |= (1<<CS12) + (0<<CS11) + (0<<CS10);
  ICR1 = 6249;
  OCR1A = 0;    //PIN 9
  OCR1B = 600; //PIN 10
  
  
  DDRB |= (1<<PB2);

} 
void loop() { 
  
  
  //PWM Aktivate [necessary after digitalWrite]
  //TCCR1A |= (1<<COM1A1) + (1<<COM1B1);
  TCCR1A |= 0b10100000;

  
  //PWM OFF
  //pinMode(10,INPUT);

  //PORTB &= ~(1<<PORTB2);
  //DDRB &= ~(1<<PB2);
  DDRB &= ~(0b00000100);  //PIN_10 Input
  PORTB &= ~(0b00000100); //PIN_10 No Pull-up
  
  delay(1000);
  
  
  //PWM ON
  //pinMode(10,OUTPUT);

  //PORTB |= (1<<PORTB2);
  //DDRB |= (1<<PB2);
  DDRB |= 0b00000100;   //PIN_10 Output
  PORTB |= 0b00000100;  //PIN_10 HIGH
  
  delay(1000);


  digitalWrite(10,HIGH);
  delay(1000);
}



void halb_HZ(){

  TCCR1A = (1<<COM1A1) + (1<<COM1B1) + (1<<WGM11); 
  TCCR1B = (1<<WGM13) + (1<<WGM12);
  TCCR1B |= (1<<CS12) + (0<<CS11) + (1<<CS10);
  ICR1 = 31249;
  OCR1A = 0;
  OCR1B = 16625;
  DDRB |= (1<<PB2);
}

void IR_HZ(){

  TCCR1A = (1<<COM1A1) + (1<<COM1B1) + (1<<WGM11); 
  TCCR1B = (1<<WGM13) + (1<<WGM12);
  TCCR1B |= (0<<CS12) + (0<<CS11) + (1<<CS10);
  ICR1 = 420; //-> 38kHz
  OCR1A = 0;
  OCR1B = 140;  //-> 33% Duty-Cycle
  DDRB |= (1<<PB2);
}

void TENHZ(){
  TCCR1A = (1<<COM1A1) + (1<<COM1B1) + (1<<WGM11); 
  TCCR1B = (1<<WGM13) + (1<<WGM12);
  TCCR1B |= (1<<CS12) + (0<<CS11) + (0<<CS10);
  ICR1 = 6249;
  OCR1A = 0;  //PIN 9
  OCR1B = 3124; //PIN 10
  DDRB |= (1<<PB2);
}