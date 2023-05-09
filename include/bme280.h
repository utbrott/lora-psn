#ifndef BME280_SENSOR_H
#define BME280_SENSOR_H

#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include "debug.h"
#include "globals.h"

#define SDA_PIN PB7
#define SCL_PIN PB6
#define SENSOR_ADDR 0x77
#define SEA_LEVEL_PRESSURE 1013.25

namespace bme280
{
    /**
     * @brief   Structure of data taken from the sensor. Temperature is a
     * floating point, pressure a 32 bit integer, is in relative %.
     */
    typedef struct
    {
        u16 temperature;
        u16 pressure;
        u16 humidity;
    } SensorData_t;

    /**
     * @brief Initialize BME280 sensor over I2C
     */
    void sensorInit(void);

    /**
     * @brief Reads data from the sensor
     * @param *data: data from the sensor
     */
    void readData(SensorData_t *sensorData);
}

#endif /* BME280_SENSOR_H */