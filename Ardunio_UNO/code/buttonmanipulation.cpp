#include <Arduino.h>

int SEND_PIN = 10; //D8
int RECV_PIN = 9;  //D7

int LED_PIN = 8; //D6

int BUTTON_PIN = 2;
bool BUTTON_STATE;

void setup()
{
  // irsend.begin();
  Serial.begin(115200);

  //pinMode(LED_PIN, OUTPUT);
  DDRB |= (1 << DDB0); //OUTPUT: 1  Input: 0
  
  //pinMode(BUTTON_PIN, INPUT);
  DDRD |= (0 << DDD2);
  PORTD |= (0<<PORTD2); //PULL-UP: 1  Pull-DOWN: 0
}

void loop()
{
  
  
  //BUTTON_STATE = digitalRead(BUTTON_PIN);
  BUTTON_STATE = (PIND & (1 << PD2));

  if (BUTTON_STATE == true)
  {
    //digitalWrite(LED_PIN, HIGH);
    PORTB |= (1 << PB0); //PORT-Register B -> PB0 LSB -> OR 1<<0 [an Stelle 0 ein Bit geschoben] -> OR 00000001
    delay(68);  //NEC IR 67.5ms to fully transmit the message frame
  }

  if (BUTTON_STATE == false)
  {
    //digitalWrite(LED_PIN, LOW);
    PORTB &= ~(1 << PB0); //PORT-Register B -> PB0 LSB -> AND NOT 1<<0 -> AND NOT 00000001 -> AND 11111110
    delay(68);  //NEC IR 67.5ms to fully transmit the message frame
  }
}
