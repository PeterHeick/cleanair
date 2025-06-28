#include <ArduinoJson.h>
#include "FanController.h"
#include "config.h"

void FanController::setup()
{
  pinMode(fanPin, OUTPUT);
  digitalWrite(fanPin, LOW); // Sikrer at blæseren er slukket ved opstart
}

int FanController::movingAverage(int avg, int no, int length)
{
  if (avg == 0)
  {
    return no;
  }
  return ((avg * (length - 1) + no) / length);
}

int FanController::get_avg1()
{
  return avg1;
}
int FanController::get_avg2()
{
  return avg2;
}

void FanController::loop(float airQuality)
{
  avg1 = movingAverage(avg1, (int)airQuality, config.get_len1());
  avg2 = movingAverage(avg2, (int)airQuality, config.get_len2());
  if (avg1 > avg2)
  {
    Serial.print("Luftkvaliteten er for dårlig, tænder blæseren \n");
    controlFan(true);
    fanStart = millis();
  }
  else
  {
    if (millis() - fanStart > config.getDuration())
    {
      Serial.print("Luftkvaliteten er god, slukker blæseren \n");
      controlFan(false);
    }
  }
  if (millis() - fanStart > config.getDuration())
  {
    Serial.printf("Test: Fan slukker efter et minut\n");
  }
  else
  {

    Serial.printf("Test: Fan kører\n");
  }
}

void FanController::controlFan(bool state)
{
  digitalWrite(fanPin, state ? HIGH : LOW);
}
