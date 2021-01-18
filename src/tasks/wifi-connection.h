#ifndef TASK_WIFI_CONNECTION
#define TASK_WIFI_CONNECTION

#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <ArduinoOTA.h>
#include "enums.h"
#include "config.h"
#include "config_sen.h"

extern void goToDeepSleep();

void OTAconfig() {
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
    
}

/**
 * Task: monitor the WiFi connection and keep it alive!
 * 
 * When a WiFi connection is established, this task will check it every 10 seconds 
 * to make sure it's still alive.
 * 
 * If not, a reconnect is attempted. If this fails to finish within the timeout,
 * the ESP32 is send to deep sleep in an attempt to recover from this.
 */
void keepWiFiAlive(){
    if(WiFi.status() == WL_CONNECTED){
        ArduinoOTA.handle();
        return;
    }

    serial_println(F("[WIFI] Connecting"));

    //WiFi.mode(WIFI_STA);
    //WiFi.setHostname(DEVICE_NAME);
    WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
      serial_println(F("Connection Failed! Rebooting..."));
      delay(5000);
      ESP.restart();
    }

    serial_print(F("[WIFI] Connected: "));
    serial_println(WiFi.localIP());
    //gDisplayValues.currentState = UP;
    
    serial_println(F("[WIFI] Connecting to OTA"));
    OTAconfig();
    serial_println(F("[WIFI] Connected"));
}




#endif
