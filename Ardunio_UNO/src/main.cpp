#include <Arduino.h>

#include <IRremote.h>
#include <SendIR.h> //own Library


int SEND_PIN = 10; // D8
int RECV_PIN = 9;  // D7
int LED_PIN = 8; // D6

int BUTTON_PIN = 2;
bool BUTTON_STATE;

IRrecv irrecv(RECV_PIN);
SendIR sendir(SEND_PIN);  

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

  if (irrecv.decode()) // If IR Signal is detected
  {
    Serial.println(irrecv.decodedIRData.protocol);
    Serial.println(irrecv.decodedIRData.decodedRawData, HEX); // LSB own SendIR is MSB

    IR_TEMP = irrecv.decodedIRData.decodedRawData;
    Serial.println();


    delay(100); // To prevent Crosstalk because of slow IR-Receiver in Devices -> wait
    sendir.sendNEC(IR_TEMP, 3);
    

    // Short Status Signal
    PORTB |= (1 << PB0); // PORT-Register B -> PB0 LSB -> OR 1<<0 [Place 0 one Bit shifted] -> OR 00000001
    delay(100);
    PORTB &= ~(1 << PB0); // PORT-Register B -> PB0 LSB -> AND NOT 1<<0 -> AND NOT 00000001 -> AND 11111110
  }

  
  if (BUTTON_STATE == true)
  {
    Serial.println("LAST IR SIGNAL");
    Serial.println(IR_TEMP, HEX);
    Serial.println();

    sendir.sendNEC(IR_TEMP, 3);

    PORTB |= (1 << PB0);
    delay(1);           
    PORTB &= ~(1 << PB0);
  }
  
  irrecv.resume();  // Receive ready for next IR-Code
  delay(10);  
}





//MSB to LSB Converter
unsigned long MSB;
uint32_t MSB_LSB(uint32_t x)
{
  x = ((x >> 1) & 0x55555555u) | ((x & 0x55555555u) << 1);
  x = ((x >> 2) & 0x33333333u) | ((x & 0x33333333u) << 2);
  x = ((x >> 4) & 0x0f0f0f0fu) | ((x & 0x0f0f0f0fu) << 4);
  x = ((x >> 8) & 0x00ff00ffu) | ((x & 0x00ff00ffu) << 8);
  x = ((x >> 16) & 0xffffu) | ((x & 0xffffu) << 16);
  return x;
}