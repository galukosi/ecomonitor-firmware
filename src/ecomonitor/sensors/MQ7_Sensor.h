#pragma once

#ifndef MQ7_Sensor_H
#define MQ7_Sensor_H

#include "SensorInterface.h"

#define MQ7_PIN 34
#define RL 10
#define RO_CLEAN_AIR 9.8

class MQ7_Sensor : public SensorInterface {
public:
    float readSensor() override;
};

#endif