#pragma once

#ifndef AM2320_SENSOR_H
#define AM2320_SENSOR_H

#include "SensorInterface.h"

class AM2320_Sensor : public SensorInterface {
public:
    float readSensor() override;
};

#endif