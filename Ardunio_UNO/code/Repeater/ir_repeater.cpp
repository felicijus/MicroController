#include <Arduino.h>

#include <IRremote.h>
#include <SendIR.h>

int SEND_PIN = 10; // D8
int RECV_PIN = 9;  // D7
int LED_PIN = 8; // D6

int BUTTON_PIN = 2;
bool BUTTON_STATE;

IRrecv irrecv(RECV_PIN);
SendIR sendir(SEND_PIN);  //own Library

unsigned long IR_TEMP = 0x0;


void setup()
{
  Serial.begin(115200);

  irrecv.enableIRIn();

  // pinMode(LED_PIN, OUTPUT);
  DDRB |= (1 << DDB0); // OUTPUT: 1  Input: 0

  // pinMode(BUTTON_PIN, INPUT);
  DDRD |= (0 << DDD2);
  PORTD |= (0 << PORTD2); // PULL-UP: 1  Pull-DOWN: 0
}


void loop()
{
  // BUTTON_STATE = digitalRead(BUTTON_PIN);
  BUTTON_STATE = (PIND & (1 << PD2));

  if (irrecv.decode()) // Wenn ein IR-Signal erkannt wurde,
  {
    Serial.println(irrecv.decodedIRData.protocol);
    Serial.println(irrecv.decodedIRData.decodedRawData, HEX); // LSB own SendIR is MSB

    IR_TEMP = irrecv.decodedIRData.decodedRawData;

    // MSB = reverse(IR_TEMP);
    // Serial.println(MSB, HEX);
    Serial.println();

    sendir.sendNEC(IR_TEMP, 3);
    
    PORTB |= (1 << PB0); // PORT-Register B -> PB0 LSB -> OR 1<<0 [an Stelle 0 ein Bit geschoben] -> OR 00000001
    delay(1);
    PORTB &= ~(1 << PB0); // PORT-Register B -> PB0 LSB -> AND NOT 1<<0 -> AND NOT 00000001 -> AND 11111110

    //irrecv.resume();
  }


  if (BUTTON_STATE == true)
  {
    Serial.println("LAST IR SIGNAL");
    Serial.println(IR_TEMP, HEX);
    Serial.println();

    sendir.sendNEC(IR_TEMP, 3);

    PORTB |= (1 << PB0); // PORT-Register B -> PB0 LSB -> OR 1<<0 [an Stelle 0 ein Bit geschoben] -> OR 00000001
    //PORTB &= ~(1 << PB0); // PORT-Register B -> PB0 LSB -> AND NOT 1<<0 -> AND NOT 00000001 -> AND 11111110
    delay(1);           // NEC IR 67.5ms to fully transmit the message frame
    PORTB &= ~(1 << PB0); // PORT-Register B -> PB0 LSB -> AND NOT 1<<0 -> AND NOT 00000001 -> AND 11111110
  }
  
  irrecv.resume();
  delay(1); // resume needs time
}



unsigned long MSB;
uint32_t reverse(uint32_t x)
{
  x = ((x >> 1) & 0x55555555u) | ((x & 0x55555555u) << 1);
  x = ((x >> 2) & 0x33333333u) | ((x & 0x33333333u) << 2);
  x = ((x >> 4) & 0x0f0f0f0fu) | ((x & 0x0f0f0f0fu) << 4);
  x = ((x >> 8) & 0x00ff00ffu) | ((x & 0x00ff00ffu) << 8);
  x = ((x >> 16) & 0xffffu) | ((x & 0xffffu) << 16);
  return x;
}