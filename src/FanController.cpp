#include <ArduinoJson.h>
#include "FanController.h"
#include "config.h"

void FanController::setup()
{
  pinMode(fanPin, OUTPUT);
  digitalWrite(fanPin, LOW); // Sikrer at blæseren er slukket ved opstart
  currentMode = AUTO; // NYT: Start altid i AUTO-tilstand
  fanStart = 0;
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

// NYT: Implementering af funktionen til at sætte tilstand
void FanController::setControlMode(FanControlMode newMode)
{
    currentMode = newMode;
    Serial.printf("Blæser-tilstand sat til: %d\n", currentMode);
    
    // Anvend tilstanden med det samme
    switch (currentMode) {
        case MANUAL_ON:
            controlFan(true);
            break;
        case MANUAL_OFF:
            controlFan(false);
            break;
        case AUTO:
            // I AUTO-tilstand vil loop() tage over. Man kan evt. køre en tjek med det samme.
            break;
    }
}

// NYT: Implementering af funktionen til at hente tilstand
FanControlMode FanController::getControlMode()
{
    return currentMode;
}


// ÆNDRET: loop() respekterer nu den valgte tilstand
void FanController::loop(float airQuality)
{
    // Hele logikken er nu pakket ind i en switch-sætning
    switch (currentMode)
    {
    case AUTO:
        // Denne kode kører kun, hvis vi er i AUTO-tilstand
        avg1 = movingAverage(avg1, (int)airQuality, config.get_len1());
        avg2 = movingAverage(avg2, (int)airQuality, config.get_len2());

        if (avg1 > avg2)
        {
            if (digitalRead(fanPin) == LOW) { // Tænd kun hvis den er slukket
                Serial.print("AUTO: Luftkvaliteten er for dårlig, tænder blæseren\n");
                controlFan(true);
                fanStart = millis();
            }
        }
        else
        {
            // Tjek kun om blæseren skal slukkes, hvis den er tændt
            if (digitalRead(fanPin) == HIGH) {
                if (millis() - fanStart > config.getDuration())
                {
                    Serial.print("AUTO: Luftkvaliteten er god, slukker blæseren\n");
                    controlFan(false);
                }
            }
        }
        break;

    case MANUAL_ON:
        // Gør ingenting. Blæseren er allerede tvunget tændt via setControlMode()
        // Vi kan evt. logge at den er i manual mode
        break;

    case MANUAL_OFF:
        // Gør ingenting. Blæseren er allerede tvunget slukket.
        break;
    }
}

// Denne funktion er nu privat, da den kun kaldes internt
void FanController::controlFan(bool state)
{
  digitalWrite(fanPin, state ? HIGH : LOW);
}