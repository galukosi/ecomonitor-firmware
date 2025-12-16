//
// Created by andri on 16.12.2025.
//

#include "DS18B20_Sensor.h"

DS18B20_Sensor::DS18B20_Sensor() {
    tempSensors.begin(); // ініціалізація сенсора
}

float DS18B20_Sensor::readSensor() {
    tempSensors.requestTemperatures();
    float final_value = tempSensors.getTempCByIndex(0);
    return final_value;
}