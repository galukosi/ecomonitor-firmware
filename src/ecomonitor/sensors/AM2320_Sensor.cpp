#include <Arduino.h>
#include "AM2320_Sensor.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_AM2320.h>

Adafruit_AM2320 humidSensor = Adafruit_AM2320();

float AM2320_Sensor::readSensor() {
    float final_value = humidSensor.readHumidity();
    return final_value;
}
