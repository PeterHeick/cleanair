#ifndef FanController_h
#define FanController_h

#include <Arduino.h>

// NYT: Definerer de forskellige kontrol-tilstande
enum FanControlMode {
    AUTO,
    MANUAL_ON,
    MANUAL_OFF
};

class FanController {
public:
    void setup();
    void loop(float airQuality);
    int get_avg1();
    int get_avg2();
    // NYT: Funktion til at sætte kontrol-tilstand udefra (fra web API)
    void setControlMode(FanControlMode newMode);
    // NYT: Funktion til at hente den nuværende tilstand (til web API)
    FanControlMode getControlMode();

private:
    // Den gamle controlFan bliver nu en privat hjælpefunktion
    void controlFan(bool state);
    const int fanPin = 32;
    unsigned long fanStart;
    int movingAverage(int avg, int no, int length);
    int avg1 = 0;
    int avg2 = 0;
    // NYT: Variabel til at holde styr på den nuværende tilstand
    FanControlMode currentMode; 
};

#endif