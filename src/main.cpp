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

// const char *ssid = "blackcrow_01";
const char *ssid = "Hamburgo 101 5G Nova";
const char *password = "8001017170";
AsyncWebServer server(80); // to handle the published API
int valor = 0;
Rotary r = Rotary(32, 35);

/// @brief Handle the API call
/// @param request full request
/// @param data JSON data
void postRule(AsyncWebServerRequest *request, uint8_t *data)
{
  size_t len = request->contentLength();
  Serial.println("Data: ");
  Serial.write(data, len); // Correctly print the received data
  Serial.println("\nLength: ");
  Serial.println(len);

  // Construct the received data string with the specified length
  String receivedData = "";
  for (size_t i = 0; i < len; i++)
  {
    receivedData += (char)data[i];
  }

  Serial.println("Received Data String: " + receivedData);

  // curl -X POST http://{ipaddress}/api/command -H "Content-Type: application/json" -d '{"command":"value"}'
  if (receivedData.indexOf("value") != -1)
  {

    request->send(200, "application/json", "{\"value\":\"" + String(valor) + "\"}");
    Serial.println("Command received: value");
  }

  else if (receivedData.indexOf("setValue=") != -1)
  {
    int startIndex = receivedData.indexOf("setValue=") + 9;
    int endIndex = receivedData.indexOf(' ', startIndex); // Assuming commands are space-separated

    if (endIndex == -1)
    {
      endIndex = receivedData.length();
    }

    String valueStr = receivedData.substring(startIndex, endIndex);
    int valueValue = valueStr.toInt();
    valor = valueValue;

    request->send(200, "application/json", "{\"status\":\"value set to:\"" + String(valueValue) + "}");
    Serial.println("Command received: updateInterval=" + String(valueValue));
  }
  else
  {
    request->send(400, "application/json", "{\"status\":\"invalid command\"}");
    Serial.println("Invalid command");
  }
}

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
  // Start API server
  server.on("/api/command", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
            { postRule(request, data); });

  server.begin();
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