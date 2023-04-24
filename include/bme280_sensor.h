#ifndef _BME280_SENSOR_H
#define _BME280_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include "globals.h"

namespace bme280
{
    /**
     * @brief   Structure of data taken from the sensor
     */
    typedef struct
    {
        u16 temperature;
        u16 pressure;
    } SensorData_t;
}

#endif /* _BME280_SENSOR_H */