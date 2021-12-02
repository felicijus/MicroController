#include <Arduino.h>
#include <Timer.h>

Timer Timer2;

void setup(){ 
  Serial.begin(115200);
  
  Timer2.Initialize();
  Timer2.AttatchInterrupt();
  
} 
void loop() { 
 
  //delay(100);
  //delay_micros_10(100000); //100k us = 1000ms

  //Serial.println(micros());
  Serial.println(Timer2.micros_10());
}