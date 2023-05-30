#ifndef BME280_SENSOR_H
#define BME280_SENSOR_H

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include "globals.h"
#include "debug.h"
#include "rollingavg.h"

#define SDA_PIN PB7
#define SCL_PIN PB6
#define SENSOR_ADDR 0x77

#define AVG_PERIOD 60
#define MEASURE_PERIOD 5
#define MEASURE_NUM (AVG_PERIOD / MEASURE_PERIOD)
namespace sensor
{
    /**
     * @brief Raw data read from BME280 breakout board.
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
     * @brief Reads raw data from the sensor and adds values to rolling avg
     */
    void readRaw(void);

    /**
     * @brief Updates data buffer with rolling average values
     * @param buffer Data buffer to update (reference)
     */
    void updateBuffer(BufferData_t *buffer);
}

#endif /* BME280_SENSOR_H */