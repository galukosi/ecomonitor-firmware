/*
 * File: EcoMonitor.cpp
 * Description: General device logic.
 * Author: Andriy Tymchuk
 * Created: 2025-12-17
*/

#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "EcoMonitor.h"
#include "sensors/SensorInterface.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Preferences prefs;
WebServer server(80);
HTTPClient http;

String sta_ssid = "";
String sta_password = "";
String device_id = "";
String connectionStatus = "";
String ap_password = "12345678";
String reading_time = "15";

// Device defaults
static const char* deviceName = "DEVICE_NAME"; // Default device name
static const char* devicePrefix = "XX";    // Default device prefix for ID generation
static const char* measurementUnit = "units"; // Default measurement unit
static String api_base_url = "https://ecomonitor-znv9.onrender.com/api"; // Default API base URL that device will send data to

bool screenEnabled = true;
bool isConfigured = false;
unsigned long lastReading = 0;
unsigned long readingInterval = 0;
unsigned long previousMillis = 0;
const unsigned long displayReadingInterval = 5000;

void setDeviceName(const char* name) { deviceName = name; }
void setDevicePrefix(const char* prefix) { devicePrefix = prefix; }
void setMeasurementUnit(const char* unit) { measurementUnit = unit; }
void setApiBaseUrl(const char* url) { api_base_url = url; }

const char* getDeviceName() { return deviceName; }
const char* getDevicePrefix() { return devicePrefix; }
const char* getMeasurementUnit() { return measurementUnit; }
String getApiBaseUrl() { return api_base_url; }

namespace EcoMonitor {
    SensorInterface *activeSensor = nullptr;

    void begin(SensorInterface* sensor) {
        activeSensor = sensor;
        
        Serial.print(getDeviceName());
        Serial.println("Initializing...");
        
        if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
            Serial.println(F("SSD1306 allocation failed"));
            Serial.println(F("Restarting in 5 seconds..."));
            delay(5000);
            ESP.restart();
        }
        
        display.clearDisplay();
        display.display();
        Serial.println("Display initialized");
        
        prefs.begin("config", false);
        
        // Check configuration first
        checkConfiguration();
        
        // Generate device ID
        device_id = generateDeviceID();
        
        // Get saved values
        api_base_url = prefs.getString("api_base_url", getApiBaseUrl());
        reading_time = prefs.getString("reading_time", "15");
        
        int minutes = reading_time.toInt();
        if (minutes <= 0 || minutes > 1440) {
            minutes = 15;
            reading_time = "15";
            prefs.putString("reading_time", reading_time);
        }
        
        readingInterval = (unsigned long)minutes * 60UL * 1000UL;
        
        Serial.println("Device ID: " + device_id);
        Serial.println("Reading interval: " + String(minutes) + " minutes");
        
        // Show initial message
        displayMessage(getDeviceName() , "Device ID:", device_id, "Starting...");
        delay(2000);
        
        // If configured, connect to Wi-Fi
        if (isConfigured) {
            Serial.println("Device is configured, connecting to WiFi...");
            connectToWiFi();
        } else {
            Serial.println("Device not configured, starting AP mode...");
            startAPMode();
            setupWebServer();
        }
    }

    void handleLoop() {
        server.handleClient();

        if (handleAPMode()) return;

        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= displayReadingInterval) {
            previousMillis = currentMillis;
            float final_value = activeSensor->readSensor();
            Serial.print("CO: "); Serial.print(final_value,1);
            Serial.println(getMeasurementUnit());
            displayData(final_value, connectionStatus);
        }

        if (lastReading == 0 || millis() - lastReading >= readingInterval) {
            lastReading = millis();
            float final_value = activeSensor->readSensor();
            sendDataToAPI(final_value);
            Serial.println("=== Sensor readings sent ===");
        }
    }
}


void checkConfiguration() {
    isConfigured = prefs.getBool("isConfigured");
    if (isConfigured == true) {
        sta_ssid = prefs.getString("sta_ssid");
        sta_password = prefs.getString("sta_password");
        reading_time = prefs.getString("reading_time");
        
        if (sta_ssid.length() > 0) {
            prefs.putBool("isConfigured", true);
            Serial.println("Device is configured");
        } else {
            prefs.putBool("isConfigured", false);
        }
    } else {
        prefs.putBool("isConfigured", false);
    }
}

void clearConfiguration() {
    prefs.clear();
    prefs.putBool("isConfigured", false);
    Serial.println("Configuration cleared");
}
void saveConfiguration() {
    prefs.putString("sta_ssid", sta_ssid);
    prefs.putString("sta_password", sta_password);
    prefs.putBool("isConfigured", true);
    Serial.println("Configuration saved to NVS");
}
void startAPMode() {
    Serial.println("Starting AP mode for configuration...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(getDeviceName(), ap_password);

    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
    Serial.print("AP SSID: ");
    Serial.println(getDeviceName());
}
void setupWebServer() {
    server.on("/", HTTP_GET, []() {
    String html = R"=====(
    <!DOCTYPE html>
    <html>
    <head>
        <title>WiFi Configuration</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
        body { font-family: Arial, sans-serif; margin: 40px; background-color: #f0f0f0; }
        .container { max-width: 500px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }
        h2 { color: #2c3e50; text-align: center; }
        input[type="text"], input[type="password"] { 
            width: 100%; padding: 12px; margin: 8px 0; 
            border: 1px solid #ccc; border-radius: 4px; 
            box-sizing: border-box;
        }
        input[type="submit"] { 
            width: 100%; background-color: #3498db; 
            color: white; padding: 14px; border: none; 
            border-radius: 4px; cursor: pointer; 
            font-size: 16px; margin-top: 10px;
        }
        input[type="submit"]:hover { background-color: #2980b9; }
        label { font-weight: bold; color: #34495e; }
        .device-info { background: #f8f9fa; padding: 15px; border-radius: 5px; margin-bottom: 15px; }
        </style>
    </head>
    <body>
        <div class="container">
        <h2>=====" + getDeviceName() + R"=====( WiFi Setup</h2>
        
        <div class="device-info">
            <strong>Device ID:</strong> )=====" + device_id + R"=====(<br>
            <strong>API URL:</strong> )=====" + String(getApiBaseUrl()) + R"=====(<br>
        </div>
        
        <form action="/configure" method="post">
            <label for="ssid">WiFi Network Name:</label>
            <input type="text" id="ssid" name="ssid" required placeholder="Enter your WiFi name">
            
            <label for="password">WiFi Password:</label>
            <input type="password" id="password" name="password" placeholder="Enter your WiFi password">
            
            <input type="submit" value="Save & Connect">
        </form>
        </div>
    </body>
    </html>
    )=====";
    server.send(200, "text/html", html);
    });

    // After pressing 'Save & Connect' button
    server.on("/configure", HTTP_POST, []() {
    if (server.hasArg("ssid")) {
        sta_ssid = server.arg("ssid");
        sta_password = server.arg("password");
        
        saveConfiguration();
        
        // Send success response
        String html = R"=====(
        <!DOCTYPE html>
        <html>
        <head>
        <title>Configuration Saved</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
            body { font-family: Arial, sans-serif; margin: 40px; text-align: center; background-color: #f0f0f0; }
            .success { color: #27ae60; font-size: 24px; font-weight: bold; }
            .container { max-width: 400px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }
        </style>
        </head>
        <body>
        <div class="container">
            <div class="success">Configuration Saved!</div>
            <p>Device will restart and connect in 5 seconds...</p>
        </div>
        <script>
            setTimeout(function() {
            window.location.href = "/";
            }, 5000);
        </script>
        </body>
        </html>
        )=====";
        
        server.send(200, "text/html", html);
        delay(5000);
        ESP.restart();
    } else {
        server.send(400, "text/plain", "Error: Missing WiFi Name");
    }
    });

    server.begin();
    Serial.println("HTTP server started");
}
void connectToWiFi() {
    sta_ssid = prefs.getString("sta_ssid", sta_ssid);
    sta_password = prefs.getString("sta_password", sta_password);
    Serial.println("Connecting to saved WiFi...");
    Serial.println("SSID: " + sta_ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(sta_ssid.c_str(), sta_password.c_str());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to WiFi!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());

        displayMessage("WiFi Connected!", "IP: " + WiFi.localIP().toString(), "Reading sensor...", "");
        delay(2000);

        lastReading = 0;

    } else {
        Serial.println("\nFailed to connect to WiFi. Starting AP mode...");
        displayMessage("WiFi Connection", "Failed!", "Starting AP mode...", "");
        delay(2000);
        startAPMode();
        setupWebServer();
    }
}

void displayMessage(String line1, String line2, String line3, String line4) {
    if (!screenEnabled) return;

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println(line1);
    display.setCursor(0,10);
    display.println(line2);
    display.setCursor(0,20);
    display.println(line3);
    display.setCursor(0,30);
    display.println(line4);
    display.display();
}
void handleApiCommand(String command, String payload) {
    Serial.println("Executing command: " + command + " | Payload: " + payload);
    if (command == "disable_screen") {
        // Disable screen command
        screenEnabled = false; // Mark screen as disabled
        display.ssd1306_command(SSD1306_DISPLAYOFF); // Disable screen using Adafruit library
        Serial.println("Screen disabled by command");
    }
    else if (command == "enable_screen") {
        // Enable screen command
        screenEnabled = true; // Mark screen as enabled
        display.ssd1306_command(SSD1306_DISPLAYON); // Enable screen using Adafruit library
        Serial.println("Screen enabled by command");
    }
    else if (command == "reboot") {
        // Reboot ESP command
        displayMessage("Rebooting...", "", "", "");
        delay(2000);
        ESP.restart(); // Restart ESP
    }
    else if (command == "change_reading_time") {
        // This command updates the reading interval using the value provided in the payload
        prefs.putString("reading_time", payload); // Save new interval to NVS
        displayMessage("Reading time", "changed to " + payload + "m", "Restarting...", "");
        delay(2000);
        ESP.restart(); // ESP m
    }
    else if (command == "factory_reset") {
        // Function for clearing the ESP NVS
        clearConfiguration();
        displayMessage("Factory Reset", "Restarting...", "", "");
        delay(2000);
        ESP.restart(); // Restart ESP
    }
    else {
        Serial.println("Unknown command: " + command);
    }
}

String generateDeviceID() {
    String id = getDevicePrefix(); // e.g., "GG-"
    id += String((uint32_t)ESP.getEfuseMac(), HEX); // *This function gets only the last 32 bits of MAC address, so in very rare situations the ID's may repeat
    id.toUpperCase();
    return id;
}

void displayData(float final_value, String connectionStatus) {
    if (!screenEnabled) return;

        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0,0);

    // Header
    if (WiFi.getMode() == WIFI_MODE_AP) {
        display.print(getDeviceName());
        display.println(" - AP Mode");
    } else {
        display.print(device_id);
        display.print(" - ");
        display.println(connectionStatus);
    }
        display.drawLine(0, 12, 128, 12, WHITE);

        display.setCursor(0,20);
        display.setTextSize(2);
    if (final_value < 0.01) {
        display.print("<0.01");
    } else if (final_value < 1.0) {
        display.print(final_value, 3);
    } else if (final_value < 10.0) {
        display.print(final_value, 2);
    } else {
        display.print(final_value, 1);
    }
    display.setTextSize(1);
    display.println(getMeasurementUnit());

    display.display();
}

void sendDataToAPI(float final_value) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Cannot send data - Wi-Fi not connected");
        connectionStatus = "Offline"; // Connection status is offline when can not connect to the API but Wi-Fi
        return;
    }

    String url = String(getApiBaseUrl()) + "/sensor-readings/";
    Serial.println("Sending to: " + url);

    JsonDocument doc;
    doc["device_id"] = device_id;
    doc["final_value"] = final_value;

    String payload;
    serializeJson(doc, payload);
    Serial.println("Payload: " + payload);

    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(10000);

    Serial.println("Sending POST request...");
    int httpCode = http.POST(payload);

    if (httpCode > 0) {
        String response = http.getString();
        Serial.println("HTTP Code: " + String(httpCode) + ", Response: " + response);
        connectionStatus = "Online";

        if (response.length() > 2) {
            JsonDocument resDoc;
            DeserializationError error = deserializeJson(resDoc, response);

            if (!error && resDoc["command"].is<String>()) {
                // If command found in response
                String command = resDoc["command"];
                String payload = resDoc["payload"] | "";
                Serial.println("Found command in response: " + command);
                handleApiCommand(command, payload);
            }
        }
    } else {
        Serial.println("HTTP POST failed: " + http.errorToString(httpCode));
        connectionStatus = "Offline";
    }

    http.end();
}

bool handleAPMode() {
    bool isConfig = prefs.getBool("isConfigured", false);  // Use different name

    if (WiFi.getMode() == WIFI_STA && WiFi.status() == WL_CONNECTED) {
        return false;
    }

    if (!isConfig) {
        static unsigned long lastDisplayUpdate = 0;
        if (millis() - lastDisplayUpdate > 2000) {
            lastDisplayUpdate = millis();
            displayMessage("AP Mode Active", 
                          String("SSID: ") + String(getDeviceName()),
                          "IP: " + WiFi.softAPIP().toString(), 
                          "Password: " + ap_password);
        }
        
        // Only start AP and server once
        static bool apStarted = false;
        if (!apStarted) {
            startAPMode();
            setupWebServer();
            apStarted = true;
            Serial.println("AP Mode started for configuration");
        }
        return true;
    }
    return false;
}
