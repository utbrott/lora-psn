#include "lora.h"
#include "bme280.h"
#include "debug.h"

HardwareSerial SerialLora(UART1_RX, UART1_TX);

namespace lora
{

    void shieldInit(lora::BoardType_t type)
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

    void sendRequest(void)
    {
        u8 message[] = {0xff};
        debug::println(debug::INFO, "Sending new requst with value 0xFF...");

        loraRadio.write(message, sizeof(message));
    }

    // Only for SLAVE modules
    void sendResponse(sensor::BufferData_t *data)
    {
        u8 message[6]; // Payload

        // Split each 16-bit field into 2x 8-bit with bit masking
        message[0] = (data->temperature & 0xff00) >> 8;
        message[1] = (data->temperature & 0x00ff);
        message[2] = (data->pressure & 0xff00) >> 8;
        message[3] = (data->pressure & 0x00ff);
        message[4] = (data->humidity & 0xff00) >> 8;
        message[5] = (data->humidity & 0x00ff);

        debug::println(debug::INFO, "Sending response with payload...");

        u8 dataSent = loraRadio.write(message, sizeof(message));
        Serial.println(dataSent);
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