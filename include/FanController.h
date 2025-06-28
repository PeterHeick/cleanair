#ifndef FanController_h
#define FanController_h

#include <Arduino.h>

class FanController {
    public:
        void setup();
        void loop(float airQuality);
        void controlFan(bool state);
        int get_avg1();
        int get_avg2();
    private:
        const int fanPin = 32;
        unsigned long fanStart;
        int movingAverage(int avg, int no, int length);
        int avg1 = 0;
        int avg2 = 0;
};

#endif
