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

    void updateBuffer(BufferData_t *buffer)
    {
        buffer->temperature = (u16)(temperatureAvg.getAverage() * 100.0f);
        buffer->pressure = (u16)(pressureAvg.getAverage() / 100.0f);
        buffer->humidity = (u16)(humidityAvg.getAverage() * 100.0f);
    }
}