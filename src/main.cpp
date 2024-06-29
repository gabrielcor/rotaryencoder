/*
    Rotary Encoder - Polling Example
    
    The circuit:
    * encoder pin A to Arduino pin 2
    * encoder pin B to Arduino pin 3
    * encoder ground pin to ground (GND)
       * Clock: 35
    * Data: 32 
*/
#include <Arduino.h>
#include <Rotary.h>

int valor = 0;
Rotary r = Rotary(32, 35);

void setup() {
  Serial.begin(9600);
  r.begin(true);
}

void loop() {
  unsigned char result = r.process();
  if (result) {
    if (result == DIR_CW)
    {
      valor++;
    }
    else
    {
      valor--;
    }
    // Serial.println(result == DIR_CW ? "Right" : "Left");
    Serial.println(valor);
  }
}