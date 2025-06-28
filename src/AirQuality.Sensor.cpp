#include "AirQualitySensor.h"

void AirQualitySensor::setup()
{
  Wire.setClock(100000);
  initialized = false;
  int initTries = 0;
  unsigned long nextAttemptTime = 0;
}

void AirQualitySensor::loop()
{
  unsigned long endTime;
  unsigned long currentMillis = millis();
  if (!initialized)
  {
    if (currentMillis >= nextAttemptTime)
    {
      if (bme.begin(0x76))
      {
        initialized = true;
        initTries = 0;
        bme.setTemperatureOversampling(BME680_OS_8X);
        bme.setHumidityOversampling(BME680_OS_2X);
        bme.setPressureOversampling(BME680_OS_4X);
        bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
        bme.setGasHeater(320, 150); // 320*C i 150 ms
        error = "";
      }
      else
      {
        unsigned long delayTime = initialDelay * pow(2, min(++initTries - 1, 8)); // Eksponentiel vækst i ventetid
        delayTime = min(delayTime, maxDelay);                                     // Begræns ventetiden til maxDelay
        nextAttemptTime = currentMillis + delayTime;
        error = "Kunne ikke finde en gyldig BME680 sensor, tjek ledninger!";
        Serial.println(error);
 
        return;
      }
    }
  }

  endTime = bme.beginReading();
  if (endTime == 0)
  {
    Serial.println(F("Failed to begin reading :("));
    return;
  }
  if (!bme.endReading())
  {
    Serial.println(F("Failed to complete reading :("));
    return;
  }
  Serial.print(F("Reading completed at "));
  Serial.println(millis());

  Serial.print(F("Temperature = "));
  temperature = bme.temperature;
  Serial.print(temperature);
  Serial.println(F(" *C"));

  Serial.print(F("Pressure = "));
  pressure = bme.pressure / 100.0;
  Serial.print(pressure);
  Serial.println(F(" hPa"));

  Serial.print(F("Humidity = "));
  humidity = bme.humidity;
  Serial.print(humidity);
  Serial.println(F(" %"));

  Serial.print(F("Gas = "));
  airQuality = bme.gas_resistance / 1000.0;
  Serial.print(airQuality);
  Serial.println(F(" KOhms"));

  Serial.print(F("Approx. Altitude = "));
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  Serial.print(altitude);
  Serial.println(F(" m"));
}

float AirQualitySensor::getTemperature()
{
  return temperature;
}
float AirQualitySensor::getPressure()
{
  return pressure;
}
float AirQualitySensor::getHumidity()
{
  return humidity;
}
float AirQualitySensor::getAirQuality()
{
  return airQuality;
}
float AirQualitySensor::getAltitude()
{
  return altitude;
}
String AirQualitySensor::getError()
{
  return error;
}