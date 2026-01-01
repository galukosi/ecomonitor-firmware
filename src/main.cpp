/*
EcoMonitor firmware
  Device: ESP32
  Author: Andriy Tymchuk
  Repository: https://github.com/trinity-corp/ecomonitor-firmware/
*/

#include <Arduino.h>
#include "ecomonitor/EcoMonitor.h"
#include "devices/GasGuard.h"
#include "devices/TempGuard.h"
#include "devices/HumidGuard.h"

void setup() {
    Serial.begin(115200);

    delay(1000);

    #ifdef GASGUARD
        setupGasGuard();
    #endif
    #ifdef TEMPGUARD
        setupTempGuard();
    #endif
    #ifdef HUMIDGUARD
        setupHumidGuard();
    #endif
}

void loop() {
    EcoMonitor::handleLoop();
    delay(10);  // Small delay to prevent watchdog issues
}