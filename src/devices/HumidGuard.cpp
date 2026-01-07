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
#include "Adafruit_Sensor.h"
#include "Adafruit_AM2320.h"

#include "HumidGuard.h"

#include "ecomonitor/EcoMonitor.h"
#include "ecomonitor/sensors/AM2320_Sensor.h"

static AM2320_Sensor am2320_sensor;

void setupHumidGuard() {
    setDeviceName("HumidGuard");
    setDevicePrefix("HG-");
    setMeasurementUnit(" % RH");
    setApiBaseUrl("https://ecomonitor-znv9.onrender.com/api");

    EcoMonitor::begin(&am2320_sensor);
}
