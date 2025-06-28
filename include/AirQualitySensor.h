#ifndef AirQualitySensor_h
#define AirQualitySensor_h

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>

#define SEALEVELPRESSURE_HPA (1013.25)

class AirQualitySensor
{
public:
  void setup();
  void loop();
  float getTemperature();
  float getPressure();
  float getHumidity();
  float getAirQuality();
  float getAltitude();
  String getError();

private:
  Adafruit_BME680 bme;
  float temperature;
  float pressure;
  float humidity;
  float airQuality;
  float altitude;
  bool initialized;
  String error;
  unsigned long nextAttemptTime;
  int initTries;
  const unsigned long initialDelay = 5000;
  const unsigned long maxDelay = 300000;
};

#endif
