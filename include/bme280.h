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

namespace sensor
{
    /**
     * @brief Raw data read from sensors of the BME280 breakout board.
     * Everything in floating point.
     */
    typedef struct
    {
        f32 temperature;
        f32 pressure;
        f32 humidity;
    } RawData_t;

    /**
     * @brief Format of data in buffer, ready to be sent with LoRa protocol.
     */
    typedef struct
    {
        u16 temperature;
        u16 pressure;
        u16 humidity;
    } BufferData_t;

    /**
     * @brief Initialize BME280 sensor over I2C
     */
    void init(void);

    /**
     * @brief Reads raw data from the sensor
     * @param *data: data from the sensor
     */
    void readRaw(RawData_t *sensorData);

    bool compareValues(RawData_t *current, RawData_t *measured);

    void updateBuffer(BufferData_t *buffer, RawData_t *raw);
}

#endif /* BME280_SENSOR_H */