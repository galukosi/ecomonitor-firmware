//
// Created by andri on 16.12.2025.
//

#include <Wire.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "TempGuard.h"

#include "ecomonitor/EcoMonitor.h"
#include "ecomonitor/sensors/DS18B20_Sensor.h"

static DS18B20_Sensor ds18b20_sensor;

void setupTempGuard() {
    setDeviceName("TempGuard");
    setDevicePrefix("TG-");
    setMeasurementUnit(" °C");
    setApiBaseUrl("https://ecomonitor-znv9.onrender.com/api");

    EcoMonitor::begin(&ds18b20_sensor);
}