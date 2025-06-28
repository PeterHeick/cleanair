#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "WebAPI.h"
#include "AirQualitySensor.h"
#include "FanController.h"
#include "config.h"

// #define NET "Skynet"
// #define PW "4PFxWBjX"
#define NET "Nauti-Net"
#define PW "&g4#wxzg"

// Ekstern reference til objekter for at kunne tilgå dem i API'et
extern AirQualitySensor airQualitySensor;
extern FanController fanController;

AsyncWebServer server(80);
bool serverStarted = false;

void WebAPI::setup()
{
  IPAddress local_IP(192, 168, 0, 38);
  IPAddress gateway(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);
  // bool connected = false;
  // int connectTries = 0;
  // unsigned long nextAttemptTime = 0;

  if (!WiFi.config(local_IP, gateway, subnet))
  {
    Serial.println("STA Failed to configure");
  }
  WiFi.begin(NET, PW);
}

bool WebAPI::connectWIFI()
{
  int retries = 10;
  while (WiFi.status() != WL_CONNECTED && retries > 0)
  {
    delay(2000);
    Serial.print("Opretter forbindelse til WiFi... Status: ");
    Serial.println(WiFi.status());
    retries--;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.printf("Connected to WIFI \"%s\" IP address: ", NET);
    Serial.println(WiFi.localIP());
    return true;
  }
  else
  {
    Serial.println("Kunne ikke forbinde til WiFi.");
    return false;
  }
}

void WebAPI::startServer()
{
  server.on("/environmentData", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
              JsonDocument doc; 
              String error = airQualitySensor.getError();
              if (error.isEmpty()) {
                doc["temperature"] = airQualitySensor.getTemperature();
                doc["pressure"] = airQualitySensor.getPressure();
                doc["humidity"] = airQualitySensor.getHumidity();
                doc["airQuality"] = airQualitySensor.getAirQuality();
                doc["altitude"] = airQualitySensor.getAltitude();
                doc["movavg1"] = fanController.get_avg1();
                doc["movavg2"] = fanController.get_avg2();
                doc["avglen1"] = config.get_len2();
                doc["avglen2"] = config.get_len2();
                doc["fanDuration"] = config.getDuration();
              } else {
                doc["temperature"] = 0;
                doc["pressure"] = 0;
                doc["humidity"] = 0;
                doc["airQuality"] = 0;
                doc["altitude"] = 0;
                doc["movavg1"] = 0;
                doc["movavg2"] = 0;
                doc["avglen1"] = config.get_len1();
                doc["avglen2"] = config.get_len2();
                doc["fanDuration"] = config.getDuration();
                doc["error"] = airQualitySensor.getError();
              }
              String jsonResponse;
              serializeJson(doc, jsonResponse);
              request->send(200, "application/json", jsonResponse); });

  server.on(
      "/avglen1", HTTP_PUT, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
      {
    JsonDocument doc;
    deserializeJson(doc, (char*)data);
    if (doc.containsKey("value")) {
        int newValue = doc["value"];
        config.put_len1(newValue);
        request->send(200, "application/json", "{\"message\":\"Threshold opdateret\"}");
    } else {
        request->send(400, "application/json", "{\"error\":\"Ugyldig forespørgsel. Ingen værdi angivet.\"}");
    } });

  server.on(
      "/avglen2", HTTP_PUT, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
      {
    JsonDocument doc;
    deserializeJson(doc, (char*)data);
    if (doc.containsKey("value")) {
        int newValue = doc["value"];
        config.put_len2(newValue);
        request->send(200, "application/json", "{\"message\":\"Threshold opdateret\"}");
    } else {
        request->send(400, "application/json", "{\"error\":\"Ugyldig forespørgsel. Ingen værdi angivet.\"}");
    } });

  server.on(
      "/duration", HTTP_PUT, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
      {
    JsonDocument doc;
    deserializeJson(doc, (char*)data);
    if (doc.containsKey("value")) {
        int newValue = doc["value"];
        config.putDuration(newValue);
        request->send(200, "application/json", "{\"message\":\"FanDuration opdateret\"}");
    } else {
        request->send(400, "application/json", "{\"error\":\"Ugyldig forespørgsel. Ingen værdi angivet.\"}");
    } });

  server.on("/controlFan", HTTP_GET, [](AsyncWebServerRequest *request)
            {
        if (request->hasParam("state")) {
            auto state = request->getParam("state")->value().toInt();
            fanController.controlFan(state > 0);
            request->send(200, "text/plain", "Blæseren er opdateret");
        } else {
            request->send(400, "text/plain", "Ugyldig forespørgsel");
        } });

  server.begin();
  serverStarted = true;
  Serial.println("Server started.");
}

void WebAPI::loop()
{
  unsigned long currentMillis = millis();
  if (!connected)
  {
    if (currentMillis >= nextAttemptTime)
    {
      if (connectWIFI())
      {
        connected = true;
        connectTries = 0;
        Serial.println("Forbundet til Wi-Fi!");
        if (!serverStarted)
        {
          startServer();
        }
      }
      else
      {
        unsigned long delayTime = initialDelay * pow(2, min(++connectTries - 1, 8)); // Eksponentiel vækst i ventetid
        delayTime = min(delayTime, maxDelay);                                        // Begræns ventetiden til maxDelay
        nextAttemptTime = currentMillis + delayTime;
        Serial.print("Forbindelsesforsøg fejlede, prøver igen om ");
        Serial.print(delayTime / 1000);
        Serial.println(" sekunder.");
      }
    }
  }
}
