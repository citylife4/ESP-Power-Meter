#include "EmonLib.h"
#include <ESP8266WiFi.h>

// Configurations
#include "config.h"
#include "enums.h"

#include "tasks/wifi-connection.h"
#include "tasks/mqtt.h"
#include "tasks/measure-eletricity.h"
#include "tasks/ntp.h"

EnergyMonitor emon1;

// Place to store local measurements before sending them off to AWS
float measurements[LOCAL_MEASUREMENTS];
unsigned char measureIndex = 0;

void setup()
{
  #if DEBUG == true
    Serial.begin(115200);
  #endif 

  // Setup the ADC
  pinMode(ADC_INPUT, INPUT);

  // Initialize emon library
  emon1.current(ADC_INPUT, ADC_CALIB);
}

void loop()
{
  // ----------------------------------------------------------------
  // TASK: Connect to WiFi & keep the connection alive.
  // ----------------------------------------------------------------
  keepWiFiAlive();
  // ----------------------------------------------------------------
  // TASK: Connect to AWS & keep the connection alive.
  // ----------------------------------------------------------------
  keepMQTTConnectionAlive();

  // ----------------------------------------------------------------
  // Task: measure electricity consumption ;)
  // ----------------------------------------------------------------
  measureElectricity();

  // ----------------------------------------------------------------
  // TASK: update time from NTP server.
  // ----------------------------------------------------------------
  //fetchTimeFromNTP();

  // ----------------------------------------------------------------
  // TASK: update WiFi signal strength
  // ----------------------------------------------------------------
}