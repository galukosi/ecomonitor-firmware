//
// Created by andri on 16.12.2025.
//

#pragma once

#ifndef DS18B20_SENSOR_H
#define DS18B20_SENSOR_H

#include <OneWire.h>
#include <DallasTemperature.h>

#include "SensorInterface.h"

#define ONE_WIRE_BUS 19 // Pin that DS18B20 connected to

class DS18B20_Sensor : public SensorInterface {
    public:
        DS18B20_Sensor();
        float readSensor() override;
        void begin();

    private:
        OneWire oneWire{ONE_WIRE_BUS};
        DallasTemperature tempSensors{&oneWire};
};

#endif