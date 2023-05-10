#include "bme280.h"

Adafruit_BME280 bme;

namespace sensor
{
    void init(void)
    {
        debug::println(debug::INFO, "Trying to init BME280 sensor");

        Wire.setSDA(SDA_PIN);
        Wire.setSCL(SCL_PIN);

        if (!bme.begin(SENSOR_ADDR))
        {
            debug::println(debug::ERR, "BME280 init fail (disconneted?)");
            while (true)
            {
                // Pause forever
            }
        }

        debug::println(debug::INFO, "BME280 sensor connected.");
    }

    void readRaw(RawData_t *data)
    {
        data->temperature = bme.readTemperature();
        data->pressure = (bme.readPressure() / 100.0f); // 1Pa accuracy is not needed, round to hPa
        data->humidity = bme.readHumidity();
    }

    bool compareValues(RawData_t *current, RawData_t *measured)
    {
        // Get relative diff between current and measured
        u16 temperatureDiff = abs(measured->temperature - current->temperature);
        u16 pressureDiff = abs(measured->pressure - current->pressure);
        u16 humidityDiff = abs(measured->humidity - current->humidity);

        bool didChange = false;

        // Temperature: update if changed by more than 0.5 deg C
        if (temperatureDiff > 0.5)
        {
            current->temperature = measured->temperature;
            didChange = true;
        }

        // Pressure: update if changed by more than 1hPa
        if (pressureDiff > 1)
        {
            current->pressure = measured->pressure;
            didChange = true;
        }

        // Humidity: update if changed by more than 0.5%
        if (humidityDiff > 0.5)
        {
            current->humidity = measured->humidity;
            didChange = true;
        }

        return didChange;
    }

    void updateBuffer(BufferData_t *buffer, RawData_t *raw)
    {
        buffer->temperature = (u16)(raw->temperature * 100.0f);
        buffer->pressure = (u16)(raw->pressure);
        buffer->humidity = (u16)(raw->humidity * 100.0f);
    }
}