#ifndef config_h
#define config_h

#include <Arduino.h>
#include <nvs_flash.h>

#define MOVAVGLEN1 50
#define MOVAVGLEN2 75
#define FAN_DURATION 60000

class Config
{
public:
  void setup();
  int get_len1();
  int get_len2();
  int32_t read();
  int32_t getDuration();
  void put_len1(int len1);
  void put_len2(int len2);
  void putDuration(int32_t duration);
private:
  nvs_handle_t handle;
  int mov_avg_len1;
  int mov_avg_len2;
  int32_t fanDuration;
};

extern Config config;
#endif
