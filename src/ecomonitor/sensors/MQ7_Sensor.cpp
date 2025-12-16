#include <Arduino.h>
#include "MQ7_Sensor.h"

float MQ7_Sensor::readSensor() {
    int sensorValue = analogRead(MQ7_PIN);
    float voltage = sensorValue * (3.3 / 4095.0);
    float RS = ((5.0 - voltage) / voltage) * RL;
    float ratio = RS / RO_CLEAN_AIR;
    float final_value = 100 * pow(ratio, -2.95); 
    return final_value;
}
