#include <Arduino.h>

void setup() {
  DDRB = 0b00000001;
}
void loop() {
  PORTB |= (1<<PB0);
  delay(1000);
  PORTB &= ~(1<<PB0);

  
  //digitalWrite(3, HIGH);
  //digitalWrite(3, LOW);

  
}