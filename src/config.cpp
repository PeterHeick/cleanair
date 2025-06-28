#include "config.h"

void Config::setup()
{
  // // Hent tærskelværdien fra NVS efter genstart
  nvs_flash_init();
  nvs_open("storage", NVS_READWRITE, &handle);

  esp_err_t err = nvs_get_i32(handle, "MovAvgLen1", &mov_avg_len1);
  if (err == ESP_ERR_NVS_NOT_FOUND)
  {
    Serial.printf("AirQuality not found in NVS, setting to default value %d\n", err);
    mov_avg_len1 = MOVAVGLEN1;
    nvs_set_i32(handle, "MovAvgLen1", mov_avg_len1);
    err = nvs_commit(handle);
    if (err != ESP_OK)
      Serial.printf("Save moving avg len 1 Failed! %s\n", err);
    else
      Serial.printf("moving avg len 1 saved %d\n", mov_avg_len1);
  }
  else if (err != ESP_OK)
  {
    Serial.printf("Error reading MovAvgLen1 from NVS %d\n", err);
  }

  err = nvs_get_i32(handle, "MovAvgLen2", &mov_avg_len2);
  if (err == ESP_ERR_NVS_NOT_FOUND)
  {
    Serial.printf("AirQuality not found in NVS, setting to default value %d\n", err);
    mov_avg_len2 = MOVAVGLEN2;
    nvs_set_i32(handle, "MovAvgLen1", mov_avg_len2);
    err = nvs_commit(handle);
    if (err != ESP_OK)
      Serial.printf("Save moving avg len 1 Failed! %s\n", err);
    else
      Serial.printf("moving avg len 1 saved %d\n", mov_avg_len2);
  }
  else if (err != ESP_OK)
  {
    Serial.printf("Error reading MovAvgLen1 from NVS %d\n", err);
  }
  Serial.printf("MovAvgLen1 threshold: %d\n", mov_avg_len1);
  Serial.printf("MovAvgLen2 threshold: %d\n", mov_avg_len2);

  err = nvs_get_i32(handle, "FanDuration", &fanDuration);
  if (err == ESP_ERR_NVS_NOT_FOUND)
  {
    Serial.printf("FanDuration not found in NVS, setting to default value %d\n", err);
    fanDuration = FAN_DURATION;
    nvs_set_i32(handle, "FanDuration", fanDuration);
    err = nvs_commit(handle);
    if (err != ESP_OK)
      Serial.printf("Save fanDuration Failed! %s\n", err);
    else
      Serial.printf("fanDuration saved %d\n", fanDuration);
  }
  else if (err != ESP_OK)
  {
    Serial.printf("Error reading fanDuration from NVS %d\n", err);
  }
  Serial.printf("fanDuration threshold: %d\n", fanDuration);
}

int Config::get_len1()
{
  return mov_avg_len1;
}

int Config::get_len2()
{
  return mov_avg_len2;
}

void Config::put_len1(int len)
{
  mov_avg_len1 = len;
  nvs_set_i32(handle, "MovAvgLen1", (int32_t) len);
  nvs_commit(handle);
}

void Config::put_len2(int len)
{
  mov_avg_len2 = len;
  nvs_set_i32(handle, "MovAvgLen1", (int32_t) len);
  nvs_commit(handle);
}

int32_t Config::getDuration()
{
  return fanDuration;
}

void Config::putDuration(int32_t duration)
{
  fanDuration = duration;
  nvs_set_i32(handle, "FanDuration", duration);
  nvs_commit(handle);
}

// int32_t Config::read()
// {
//   int32_t saved = -1;
//   esp_err_t err = nvs_get_i32(handle, "AirQuality", &saved);
//   if (err == ESP_ERR_NVS_NOT_FOUND)
//   {
//     Serial.println("AirQuality not found in NVS");
//   }
//   return saved;
// }