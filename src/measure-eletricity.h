#ifndef TASK_MEASURE_ELECTRICITY
#define TASK_MEASURE_ELECTRICITY

#include <Arduino.h>
#include "EmonLib.h"

#include "config.h"
#include "enums.h"
#include "mqtt.h"

extern EnergyMonitor emon1;
extern float measurements[];
extern unsigned char measureIndex;

void measureElectricity()
{
    //serial_println("[ENERGY] Measuring...");
    //long start = millis();

    double amps = emon1.calcIrms(1480);
    double watts = amps * HOME_VOLTAGE;

    //serial_print("[ENERGY] Amps= ");
    serial_println(amps);
    //serial_print("[ENERGY] Watts=");
    //serial_println(watts);

    measurements[measureIndex] = watts;
    measureIndex++;

    if (measureIndex == LOCAL_MEASUREMENTS)
    {
        uploadMeasurementsToAWS();
        measureIndex = 0;
    }

    //long end = millis();

    // Schedule the task to run again in 1 second (while
    // taking into account how long measurement took)
    // vTaskDelay((1000-(end-start)) / portTICK_PERIOD_MS);
}

#endif
