#include "bme280_sensor.h"
#include "debug.h"

Adafruit_BME280 sensor;

void bme280::sensorInit(void)
{
    debug::printDebug(debug::INFO, "Trying to init BME280 sensor");

    Wire.setSDA(SDA_PIN);
    Wire.setSCL(SCL_PIN);

    if (!sensor.begin(SENSOR_ADDR))
    {
        debug::printDebug(debug::ERR, "BME280 not found! Connect it and restart board.");
        while (1)
        {
            // Infinite loop, do not continue if no sensor found
        }
    }

    debug::printDebug(debug::INFO, "BME280 sensor connected.");
}

void bme280::readData(bme280::SensorData_t *data)
{
    // Change the magnitue for easier sending
    data->temperature = (sensor.readTemperature() * 100);
    data->pressure = (sensor.readPressure() / 100.00f);
}