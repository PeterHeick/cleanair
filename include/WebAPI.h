#ifndef WebAPI_h
#define WebAPI_h

#include <Arduino.h>
#include <WiFi.h>

class WebAPI
{
public:
  void setup();
  void loop();
  
private: // Gør funktioner private, da de kun kaldes internt
  bool connectWIFI();
  void startServer();
  void checkConnection(); // NYT: Funktion til at tjekke forbindelsens sundhed

  bool connected;
  bool serverStarted; // Flyttet fra global i .cpp
  int connectTries;
  unsigned long nextAttemptTime;
  unsigned long lastConnectionCheck; // NYT: Timer for sundhedstjek

  const unsigned long initialDelay = 5000;
  const unsigned long maxDelay = 300000;
};

#endif