/*
 * File: main.cpp
 * Description: Controls the start of the system and delegates all work to the modules.
 * Author: Andriy Tymchuk
 * Created: 2025-12-17
 * Note: Almost all files was created on the same date. That's because the code were transfered from Arduino IDE to PlatformIO.
*/

#include <Arduino.h>
#include "ecomonitor/EcoMonitor.h"
#include "devices/GasGuard.h"
#include "devices/TempGuard.h"
#include "devices/HumidGuard.h"

void setup() {
    Serial.begin(115200);

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