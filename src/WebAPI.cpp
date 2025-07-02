#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <ESP32Ping.h> // NYT: Inkluder ping-biblioteket
#include "WebAPI.h"
#include "AirQualitySensor.h"
#include "FanController.h"
#include "config.h"

// Dine WiFi-definitioner
#define NET "Network Unreachable"
#define PW "RvfpJLGDE"

// Ekstern reference til globale objekter
extern AirQualitySensor airQualitySensor;
extern FanController fanController;
extern Config config;

// Globale variabler for denne fil
AsyncWebServer server(80);

// Komplet HTML, CSS og JavaScript for brugerfladen (ingen ændringer her)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="da">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Styring</title>
    <style>
        body { font-family: Arial, sans-serif; background-color: #f4f4f4; margin: 0; padding: 20px; display: flex; justify-content: center; }
        .container { max-width: 600px; width: 100%; background-color: #fff; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
        h1, h2 { color: #333; text-align: center; }
        .data-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); gap: 15px; margin-bottom: 20px; }
        .data-item { background-color: #eef; padding: 15px; border-radius: 5px; text-align: center; }
        .data-item .label { font-size: 0.9em; color: #666; }
        .data-item .value { font-size: 1.5em; font-weight: bold; color: #0056b3; }
        .controls { text-align: center; padding: 20px; border-top: 1px solid #ddd; }
        .controls button { font-size: 1.1em; padding: 10px 15px; margin: 5px; border: none; border-radius: 5px; cursor: pointer; color: #fff; transition: background-color 0.3s; }
        .btn-on { background-color: #28a745; }
        .btn-off { background-color: #dc3545; }
        .btn-auto { background-color: #007bff; }
        button.active { border: 3px solid #333; font-weight: bold; }
        #status-bar { text-align: center; margin-top: 15px; padding: 10px; border-radius: 5px; font-weight: bold; }
        #status-bar.auto { background-color: #e6f2ff; color: #004085; }
        #status-bar.on { background-color: #e9f7ef; color: #155724; }
        #status-bar.off { background-color: #fbe6e8; color: #721c24; }
        #error-message { color: red; text-align: center; margin-bottom: 10px; font-weight: bold; }
    </style>
</head>
<body>
    <div class="container">
        <h1>ESP32 Sensor- og Blæserstyring</h1>
        <h2>Live Målinger</h2>
        <div id="error-message"></div>
        <div class="data-grid">
            <div class="data-item"><div class="label">Temperatur</div><div class="value"><span id="temp">--</span> &deg;C</div></div>
            <div class="data-item"><div class="label">Fugtighed</div><div class="value"><span id="hum">--</span> %</div></div>
            <div class="data-item"><div class="label">Tryk</div><div class="value"><span id="pres">--</span> hPa</div></div>
            <div class="data-item"><div class="label">Luftkvalitet</div><div class="value"><span id="aqi">--</span> KOhms</div></div>
        </div>
        <div class="controls">
            <h2>Blæserstyring</h2>
            <button id="btn-on" class="btn-on" onclick="setFanMode('on')">FORCE TÆND</button>
            <button id="btn-off" class="btn-off" onclick="setFanMode('off')">FORCE SLUK</button>
            <button id="btn-auto" class="btn-auto" onclick="setFanMode('auto')">AUTO MODE</button>
            <div id="status-bar">Henter status...</div>
        </div>
    </div>
    <script>
        const buttons = { on: document.getElementById('btn-on'), off: document.getElementById('btn-off'), auto: document.getElementById('btn-auto') };
        const statusBar = document.getElementById('status-bar');
        function updateStatusUI(mode) {
            Object.values(buttons).forEach(btn => btn.classList.remove('active'));
            statusBar.classList.remove('on', 'off', 'auto');
            if (mode === 'on') {
                buttons.on.classList.add('active');
                statusBar.className = 'on';
                statusBar.innerText = 'Blæser er tvunget TÆNDT';
            } else if (mode === 'off') {
                buttons.off.classList.add('active');
                statusBar.className = 'off';
                statusBar.innerText = 'Blæser er tvunget SLUKKET';
            } else {
                buttons.auto.classList.add('active');
                statusBar.className = 'auto';
                statusBar.innerText = 'Blæser er i AUTO tilstand';
            }
        }
        function fetchData() {
            fetch('/environmentData').then(response => response.json()).then(data => {
                if (data.error) {
                    document.getElementById('error-message').innerText = 'Fejl fra sensor: ' + data.error;
                } else {
                    document.getElementById('error-message').innerText = '';
                    document.getElementById('temp').innerText = parseFloat(data.temperature).toFixed(2);
                    document.getElementById('hum').innerText = parseFloat(data.humidity).toFixed(2);
                    document.getElementById('pres').innerText = parseFloat(data.pressure).toFixed(2);
                    document.getElementById('aqi').innerText = parseFloat(data.airQuality).toFixed(2);
                    const modeMap = ['auto', 'on', 'off'];
                    updateStatusUI(modeMap[data.fanMode]);
                }
            }).catch(error => {
                console.error('Fejl ved hentning af data:', error);
                document.getElementById('error-message').innerText = 'Kunne ikke forbinde til ESP32.';
            });
        }
        function setFanMode(mode) {
            fetch(`/setFanMode?mode=${mode}`).then(response => {
                if (response.ok) {
                    console.log(`Mode set to ${mode}`);
                    updateStatusUI(mode);
                } else {
                    console.error('Failed to set mode');
                }
            }).catch(error => console.error('Fejl ved styring af blæser:', error));
        }
        setInterval(fetchData, 5000);
        window.onload = fetchData;
    </script>
</body>
</html>
)rawliteral";


void WebAPI::setup()
{
  // Initialiser variabler
  connected = false;
  serverStarted = false;
  connectTries = 0;
  nextAttemptTime = 0;
  lastConnectionCheck = 0;

  // Sæt statisk IP-konfiguration
  IPAddress local_IP(192, 168, 50, 38);
  IPAddress gateway(192, 168, 50, 1);
  IPAddress subnet(255, 255, 255, 0);

  if (!WiFi.config(local_IP, gateway, subnet))
  {
    Serial.println("STA Failed to configure");
  }
  
  // Start den første forbindelsesproces
  WiFi.begin(NET, PW);
}

bool WebAPI::connectWIFI()
{
  int retries = 5; // Kortere timeout, da loop() kalder den igen
  while (WiFi.status() != WL_CONNECTED && retries > 0)
  {
    delay(1000);
    Serial.print(".");
    retries--;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.printf("\nForbundet til WIFI \"%s\" med IP: ", NET);
    Serial.println(WiFi.localIP());
    return true;
  }
  else
  {
    return false;
  }
}

void WebAPI::startServer()
{
  if (serverStarted) {
    return; // Serveren kører allerede
  }
  // Definer alle serverens endpoints
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });

  server.on("/environmentData", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              JsonDocument doc;
              doc["fanMode"] = fanController.getControlMode();
              // ... resten af din JSON-logik
              String jsonResponse;
              serializeJson(doc, jsonResponse);
              request->send(200, "application/json", jsonResponse);
            });

  server.on("/setFanMode", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              if (request->hasParam("mode")) {
                // ... resten af din setFanMode logik
              }
            });
  
  // Dine PUT-endpoints ...

  server.begin();
  serverStarted = true;
  Serial.println("Webserver startet.");
}

// NYT: Funktion til at tjekke forbindelsens sundhed
void WebAPI::checkConnection() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi er ikke forbundet. Starter genforbindelse...");
        connected = false;
        serverStarted = false; // Sørger for at serveren genstartes
        WiFi.disconnect(); // Ryd op i gammel forbindelse
        return; // Lad hoved-loopet håndtere genforbindelse
    }

    // Hvis vi er forbundet, så ping gateway for at tjekke reel forbindelse
    IPAddress gateway = WiFi.gatewayIP();
    Serial.printf("Pinger gateway: %s\n", gateway.toString().c_str());
    if(!Ping.ping(gateway, 2)) { // 2 forsøg
        Serial.println("Ping fejlede! Forbindelsen er sandsynligvis død. Genstarter WiFi...");
        connected = false;
        serverStarted = false;
        WiFi.disconnect(); // Tving en fuld genforbindelse
    } else {
        Serial.println("Ping succesfuld. Forbindelse er OK.");
    }
}


void WebAPI::loop()
{
  unsigned long currentMillis = millis();

  if (connected) {
      // Hvis vi er forbundet, lav et periodisk sundhedstjek
      if (currentMillis - lastConnectionCheck > 60000) { // Tjek hvert 60. sekund
          lastConnectionCheck = currentMillis;
          checkConnection();
      }
  } else {
      // Hvis vi IKKE er forbundet, prøv at genforbinde med eksponentiel backoff
      if (currentMillis >= nextAttemptTime)
      {
        Serial.println("Forsøger at forbinde til WiFi...");
        if (connectWIFI())
        {
          connected = true;
          connectTries = 0;
          startServer(); // Sørg for at serveren altid startes/genstartes efter forbindelse
        }
        else
        {
          unsigned long delayTime = initialDelay * pow(2, min(++connectTries - 1, 8));
          delayTime = min(delayTime, maxDelay);
          nextAttemptTime = currentMillis + delayTime;
          Serial.printf("Forbindelse fejlede. Prøver igen om %lu sekunder.\n", delayTime / 1000);
        }
      }
  }
}