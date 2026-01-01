/*
 * File: SensorInterface.h
 * Description: Header file for connecting device logic with the sensors.
 * Author: Andriy Tymchuk
 * Created: 2025-12-17
*/

#pragma once

#ifndef SENSOR_INTERFACE_H
#define SENSOR_INTERFACE_H

class SensorInterface {
public:
    virtual float readSensor() = 0;
    virtual ~SensorInterface() {}
};

#endif