#include <Arduino.h>
// #include <IRsend.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>

#include <SendIR.h>

int SEND_PIN = D8; //D8
int RECV_PIN = D7; //D7

int LED_PIN = D6; //D6

IRrecv irrecv(RECV_PIN);
// IRsend irsend(SEND_PIN);
decode_results results;

//#define NEC_HEX_VALUE 0xFF6897 //UL makes this an unsigned long
#define NEC_HEX_VALUE 0x3EC158A7


#define NEC_BIT_COUNT 32
SendIR sendir(SEND_PIN);
 
void setup()
  {
    // irsend.begin();
    Serial.begin(115200);

    irrecv.enableIRIn();

    pinMode(LED_PIN, OUTPUT);
  }
  
/* void loop() 
  { 
    if (irrecv.decode(&results))          //Wenn ein IR-Signal erkannt wurde,
      {
        delay(100);
        Serial.println(results.decode_type);
        Serial.println(results.value, HEX);   //wird der Wert im seriellen Monitor ausgegeben
        
        String a = String(results.value, HEX);
        Serial.println(a);
        
        irrecv.resume();

        if (String(results.value, HEX) == "ff30cf"){
          digitalWrite(LED_PIN, HIGH);
          delay(1000);
          

          irsend.sendNEC(0xFF6897, 32);
          //irsend.sendNEC(0x3EC158A7, 32); //Dali an
        }

        if (String(results.value, HEX) == "ff6897"){
          digitalWrite(LED_PIN, LOW);
        }
      }
    //delay(100);
  } */

  void loop() {
  
  //Next send the NEC_HEX_VALUE signal defined above
  Serial.println(F("Sending NEC_HEX_VALUE @ 38kHz"));
  sendir.sendHexNEC(NEC_HEX_VALUE, NEC_BIT_COUNT, 1, 38);
  delay(5000); //wait 5 seconds between each signal (change to suit)

}
