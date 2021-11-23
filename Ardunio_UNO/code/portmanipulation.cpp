#include <Arduino.h>


bool PIN_STATE;

void setup() {
  Serial.begin(9600);
  DDRB = 0b00000001;
}

void loop() {
  
  PORTB |= (1<<PB0);  //PORT-Register B -> PB0 LSB -> OR 1<<0 [an Stelle 0 ein Bit geschoben] -> OR 00000001

  //digitalWrite(8, HIGH);
  Serial.println("ON");
  PIN_STATE = (PINB & (1<<PB0));
  Serial.println(PIN_STATE);

  delay(1000);

  PORTB &= ~(1<<PB0); //PORT-Register B -> PB0 LSB -> AND NOT 1<<0 -> AND NOT 00000001 -> AND 11111110
  //digitalWrite(8, LOW);
  Serial.println("OFF");
  PIN_STATE = (PINB & (1<<PB0));
  Serial.println(PIN_STATE);
  
  delay(1000);

  // VINCENT IT WORKS
}