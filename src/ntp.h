#ifndef TASK_FETCH_TIME_NTP
#define TASK_FETCH_TIME_NTP

#include <WiFiUdp.h>
#include <NTPClient.h>
#include "enums.h"

extern void reconnectWifiIfNeeded();

WiFiUDP ntpUDP;

// TODO: this does not take timezones into account! Only UTC for now.
NTPClient timeClient(ntpUDP, NTP_SERVER, NTP_OFFSET_SECONDS, NTP_UPDATE_INTERVAL_MS);

void fetchTimeFromNTP()
{
    if (!WiFi.isConnected())
    {
        return;
    }

    serial_println("[NTP] Updating...");
    timeClient.update();

    String timestring = timeClient.getFormattedTime();
    short tIndex = timestring.indexOf("T");
    //gDisplayValues.time = timestring.substring(tIndex + 1, timestring.length() - 3);

    serial_println("[NTP] Done");

    // Sleep for a minute before checking again
}
#endif
