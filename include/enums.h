#ifndef ENUMS
#define ENUMS

#include <Arduino.h>

// The state in which the device can be. This mainly affects what
// is drawn on the display.
enum DEVICE_STATE
{
  CONNECTING_WIFI,
  CONNECTING_AWS,
  FETCHING_TIME,
  UP,
};

// Place to store all the variables that need to be displayed.
// All other functions should update these!
struct DisplayValues
{
  double watt;
  double amps;
  int8_t wifi_strength;
  DEVICE_STATE currentState;
  String time;
};

#define DEBUG true

#if DEBUG == true
#define serial_print(x) Serial.print(x)
#define serial_println(x) Serial.println(x)
#define DEBUG_LOG(msg) Serial.print(msg)
#define DEBUG_LOGF(fmt, p1) Serial.printf((fmt), (p1))
#define DEBUG_LOGLN(msg) Serial.println(msg)
#else
#define serial_print(x)
#define serial_println(x)
#define DEBUG_LOG(msg)
#define DEBUG_LOGF(fmt, p1)
#define DEBUG_LOGLN(msg)
#endif

#endif