#ifndef WebAPI_h
#define WebAPI_h

#include <Arduino.h>
#include <WiFi.h>
// #include <ESPAsyncWebServer.h>

class WebAPI
{
public:
  void setup();
  void loop();
  bool connectWIFI();
  void startServer();
  void connectToBestNetwork();
  void waitForConnection();

private:
  bool connected;
  int connectTries;
  unsigned long nextAttemptTime;
  const unsigned long initialDelay = 5000; // Start delay på 5 sekunder
  const unsigned long maxDelay = 300000;   // Maksimal delay på 5 minutter
};

#endif
