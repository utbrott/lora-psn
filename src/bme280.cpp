#include "bme280.h"

Adafruit_BME280 bme;
RollingAvg temperatureAvg(MEASURE_NUM);
RollingAvg pressureAvg(MEASURE_NUM);
RollingAvg humidityAvg(MEASURE_NUM);

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

        // Explicit clear RAs
        temperatureAvg.clear();
        pressureAvg.clear();
        humidityAvg.clear();
    }

    void readRaw(void)
    {
        temperatureAvg.addValue(bme.readTemperature());
        pressureAvg.addValue(bme.readPressure());
        humidityAvg.addValue(bme.readHumidity());
    }

    // NO LONGER NEEDED
    // bool compareValues(RawData_t *current, RawData_t *measured)
    // {
    //     // Get relative diff between current and measured
    //     u16 temperatureDiff = abs(measured->temperature - current->temperature);
    //     u16 pressureDiff = abs(measured->pressure - current->pressure);
    //     u16 humidityDiff = abs(measured->humidity - current->humidity);

    //     bool didChange = false;

    //     // Temperature: update if changed by more than 0.5 deg C
    //     if (temperatureDiff > 0.5)
    //     {
    //         current->temperature = measured->temperature;
    //         didChange = true;
    //     }

    //     // Pressure: update if changed by more than 1hPa
    //     if (pressureDiff > 100)
    //     {
    //         current->pressure = measured->pressure;
    //         didChange = true;
    //     }

    //     // Humidity: update if changed by more than 0.5%
    //     if (humidityDiff > 0.5)
    //     {
    //         current->humidity = measured->humidity;
    //         didChange = true;
    //     }

    //     return didChange;
    // }

    void updateBuffer(BufferData_t *buffer)
    {
        buffer->temperature = (u16)(temperatureAvg.getAverage() * 100.0f);
        buffer->pressure = (u16)(pressureAvg.getAverage() / 100.0f);
        buffer->humidity = (u16)(humidityAvg.getAverage() * 100.0f);

        /* REMOVE ME*/
        String values = "Values in buffer" + String(buffer->temperature) + " " + String(buffer->pressure) + " " + String(buffer->humidity);
        Serial.println(values);
    }
}