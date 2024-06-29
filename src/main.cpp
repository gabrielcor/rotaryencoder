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
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>

const char *ssid = "blackcrow_01";
const char *password = "8001017170";
int valor = 0;
Rotary r = Rotary(32, 35);

void setup()
{
  Serial.begin(9600);

  // Wifi setup
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Print the IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  r.begin(true);
}

void loop()
{
  unsigned char result = r.process();
  if (result)
  {
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