#include <Arduino.h>
#include "config.h"
#include "AirQualitySensor.h"
#include "FanController.h"
#include "WebAPI.h"

Config config;
AirQualitySensor airQualitySensor;
FanController fanController;
WebAPI webAPI;

void setup()
{
  Serial.begin(115200);
  config.setup();
  airQualitySensor.setup();
  fanController.setup();
  webAPI.setup();
}

void loop()
{
  airQualitySensor.loop();
  fanController.loop(airQualitySensor.getAirQuality());
  webAPI.loop();
  Serial.println("--------------------");
  delay(5000);
}
