#include <Arduino.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>

int SEND_PIN = D8; //D8
int RECV_PIN = D7; //D7

int LED_PIN = D6; //D6

IRrecv irrecv(RECV_PIN);
IRsend irsend(SEND_PIN);
decode_results results;
 
void setup()
  {
    irsend.begin();
    Serial.begin(115200);
    irrecv.enableIRIn();

    pinMode(LED_PIN, OUTPUT);
  }
  
void loop() 
  { 
    if (irrecv.decode(&results))          //Wenn ein IR-Signal erkannt wurde,
      {
        //delay(100);
  
  if (results.decode_type == UNKNOWN) {
    Serial.print("Unknown encoding: ");
  } else if (results.decode_type == NEC) {
    Serial.print("Decoded NEC: ");
  } else if (results.decode_type == SONY) {
    Serial.print("Decoded SONY: ");
  } else if (results.decode_type == RC5) {
    Serial.print("Decoded RC5: ");
  } else if (results.decode_type == RC5X) {
    Serial.print("Decoded RC5X: ");
  } else if (results.decode_type == RC6) {
    Serial.print("Decoded RC6: ");
  } else if (results.decode_type == RCMM) {
    Serial.print("Decoded RCMM: ");
  } else if (results.decode_type == PANASONIC) {
    Serial.print("Decoded PANASONIC - Address: ");
    Serial.print(results.address, HEX);
    Serial.print(" Value: ");
  } else if (results.decode_type == LG) {
    Serial.print("Decoded LG: ");
  } else if (results.decode_type == JVC) {
    Serial.print("Decoded JVC: ");
  } else if (results.decode_type == AIWA_RC_T501) {
    Serial.print("Decoded AIWA RC T501: ");
  } else if (results.decode_type == WHYNTER) {
    Serial.print("Decoded Whynter: ");
  } else if (results.decode_type == NIKAI) {
    Serial.print("Decoded Nikai: ");
  }
  
        Serial.println(results.decode_type);
        Serial.println(results.value, HEX);   //wird der Wert im seriellen Monitor ausgegeben
        
        delay(3000);
        String a = String(results.value,HEX);
        if (String(results.value, HEX) == a){
          digitalWrite(LED_PIN, HIGH);
          delay(100);
          digitalWrite(LED_PIN, LOW);
        }

        irsend.sendNEC(results.value, 32);
        irrecv.resume();
      }
      yield();
  }