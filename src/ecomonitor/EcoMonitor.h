#ifndef ECOMONITOR_ECOMONITOR_H
#define ECOMONITOR_ECOMONITOR_H

void setDeviceName(const char* name);
void setDevicePrefix(const char* prefix);
void setMeasurementUnit(const char* unit);
void setApiBaseUrl(const char* url);

extern const char* getDeviceName();
extern const char* getDevicePrefix();
extern const char* getMeasurementUnit();
String getApiBaseUrl();

#include <Arduino.h>
#include <Preferences.h>
#include <Wire.h>
#include <WebServer.h>
#include <HTTPClient.h>

class Preferences;
class Adafruit_SSD1306;
class WebServer;
class HTTPClient;
class SensorInterface;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
#define OLED_SDA 21
#define OLED_SCL 22

namespace EcoMonitor {
    extern SensorInterface *activeSensor;
    
    void begin(SensorInterface* sensor);
    void handleLoop();   
}

// Configuration
void checkConfiguration();
void saveConfiguration();
void clearConfiguration();

// Display
void displayMessage(String line1 = "", String line2 = "", String line3 = "", String line4 = "");
void displayData(float final_value, String connectionStatus);

// Sensor
float readSensor();

// WiFi and Webserver
void setupWebServer();
void startAPMode();
bool handleAPMode();
void connectToWiFi();
void handleApiCommand(String command, String payload);
void sendDataToAPI(float final_value);

// Device ID
String generateDeviceID();

#endif