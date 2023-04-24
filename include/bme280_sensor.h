#ifndef BME280_SENSOR_H
#define BME280_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "globals.h"
#include "debug.h"

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

    /**
     * @brief Initialize BME280 sensor over I2C
     */
    void sensorInit(void);

    /**
     * @brief Reads data from the sensor
     * @param *data: data from the sensor
     */
    void readData(SensorData_t *data);
}

#endif /* BME280_SENSOR_H */