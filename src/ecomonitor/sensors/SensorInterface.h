#pragma once

#ifndef SENSOR_INTERFACE_H
#define SENSOR_INTERFACE_H

class SensorInterface {
public:
    virtual float readSensor() = 0;
    virtual ~SensorInterface() {}
};

#endif