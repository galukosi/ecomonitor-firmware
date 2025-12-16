//
// Created by andri on 16.12.2025.
//

#include <WebServer.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "GasGuard.h"

#include "ecomonitor/EcoMonitor.h"
#include "ecomonitor/sensors/MQ7_Sensor.h"

static MQ7_Sensor mq7_sensor;

void setupGasGuard() {
    setDeviceName("GasGuard");
    setDevicePrefix("GG-");
    setMeasurementUnit(" ppm");
    setApiBaseUrl("https://ecomonitor-znv9.onrender.com/api");

    EcoMonitor::begin(&mq7_sensor);
}
