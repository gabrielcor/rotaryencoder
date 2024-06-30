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
// URL to send the result when the puzle is ready (all the strips selected)
String url2SendResult = "http://homeassistant.local:1880/endpoint/lab/rotarydata";
int rotaryNumber = 1;

AsyncWebServer server(80); // to handle the published API
int valor = 0;
int lastReportedValue = -3;
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
  // curl -X POST http://192.168.1.186/api/command -H "Content-Type: application/json" -d '{"command":"setValue=0"}'
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
  //curl -X POST http://192.168.1.186/api/command -H "Content-Type: application/json" -d '{"command":"setUrl=http://homeassistant.local:1880/endpoint/lab/rotarydata"}'
  else if (receivedData.indexOf("setUrl=") != -1)
  {
    int startIndex = receivedData.indexOf("setUrl=") + 7;
    int endIndex = receivedData.indexOf('}', startIndex); // Assuming commands are space-separated

    if (endIndex == -1)
    {
      endIndex = receivedData.length();
    }

    String valueStr = receivedData.substring(startIndex, endIndex);
    url2SendResult = valueStr;

    request->send(200, "application/json", "{\"status\":\"urlValue set to:\"" + String(url2SendResult) + "}");
    Serial.println("Command received: setUrl=" + valueStr);
  }

  else
  {
    request->send(400, "application/json", "{\"status\":\"invalid command\"}");
    Serial.println("Invalid command");
  }
}

void sendRotaryValue(int valueToReport)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    http.begin(url2SendResult); // Replace with your API endpoint
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"rotary\":" + String(rotaryNumber) + ",\"value\":" + String(valueToReport) + "}";
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0)
    {
      String response = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response: " + response);
    }
    else
    {
      Serial.println("Error on sending POST: " + String(httpResponseCode));
    }
    http.end();
  }
  else
  {
    Serial.println("WiFi not connected");
  }
}

TaskHandle_t Task1; // Definir el Handle para que compile
void Task1code( void * parameter) {
  Serial.print("Task1code() running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    // put your main code here, to run repeatedly:
    if (lastReportedValue != valor)
    {
       lastReportedValue = valor;
       sendRotaryValue(lastReportedValue);
    }
    delay(50);
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
  
 // Multitask setup
 xTaskCreatePinnedToCore(
      Task1code, /* Function to implement the task */
      "Task1", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Task1,  /* Task handle. */
      0); /* Core where the task should run */
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