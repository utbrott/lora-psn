#include "lora.h"
#include "bme280.h"
#include "debug.h"

HardwareSerial SerialLora(UART1_RX, UART1_TX);

namespace lora
{

    void shieldInit(BoardType_t type)
    {
        switch (type)
        {
        case SLAVE:
            debug::println(debug::INFO, "SLAVE module");
            break;

        case MASTER:
            debug::println(debug::INFO, "MASTER module");
            break;
        }

        while (!loraRadio.begin(&SerialLora))
        {
            debug::println(debug::INFO, "Shield not ready...");
            delay(1000); // Give the module 1s to init
        }

        debug::println(debug::INFO, "Shield ready!");
    }

    void sendRequest(u8 message)
    {
        u8 msg[] = {message};
        debug::println(debug::INFO, "Sending new requst with value 0x" + String(message, HEX));

        loraRadio.write(msg, sizeof(msg));
    }

    // Only for SLAVE modules
    void sendResponse(sensor::BufferData_t *buffer, u8 dataId)
    {
        u8 message[2]; // Payload
        u16 bufferValue;

        String values = "\n" + String(buffer->temperature) + " " + String(buffer->pressure) + " " + String(buffer->humidity);
        Serial.println(values);

        switch (dataId)
        {
        case TEMPERATURE:
            bufferValue = (u16)buffer->temperature;
            break;

        case PRESSURE:
            bufferValue = (u16)buffer->pressure;
            break;

        case HUMIDITY:
            bufferValue = (u16)buffer->humidity;
            break;
        }

        Serial.println(bufferValue);

        // Split 16-bit field into 2x 8-bit with bit masking
        message[0] = (bufferValue & 0xff00) >> 8;
        message[1] = (bufferValue & 0x00ff);

        debug::println(debug::INFO, "Sending response");
        loraRadio.write(message, sizeof(message));
    }

    // Only for MASTER module
    void readResponse(ReceivedData_t *data, u8 message[])
    {
        // Merge each 2x 8-bit fields into 1x 16-bit one, fix magnitudes
        data->temperature = (f32)((message[0] << 8) + message[1]) / 100;
        data->pressure = (f32)((message[2] << 8) + message[3]);
        data->humidity = (f32)((message[4] << 8) + message[5]) / 100;
        memset(message, 0, 8);

        String temperatureMsg = "Temperature: " + String(data->temperature) + "\u00b0C";
        String pressureMsg = "Pressure: " + String(data->pressure) + "hPa";
        String humidityMsg = "Humidity: " + String(data->humidity) + "%";

        String formattedMessage[4] = {
            "Response received",
            temperatureMsg,
            pressureMsg,
            humidityMsg,
        };

        for (u8 i = 0; i < ARRAYSIZE(formattedMessage); ++i)
        {
            debug::println(debug::INFO, formattedMessage[i]);
        }
    }
}