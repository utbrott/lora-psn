#include "lora.h"
#include "bme280.h"
#include "debug.h"

HardwareSerial SerialLora(UART1_RX, UART1_TX);

namespace lora
{

    void shieldInit(lora::BoardType_t type, bool setupSensor)
    {
        switch (type)
        {
        case SLAVE:
            debug::printDebug(debug::INFO, "SLAVE module");
            break;

        case MASTER:
            debug::printDebug(debug::INFO, "MASTER module");
            break;
        }

        while (!loraRadio.begin(&SerialLora))
        {
            debug::printDebug(debug::INFO, "Shield not ready...");
            delay(1000); // Give the module 1s to init
        }

        debug::printDebug(debug::INFO, "Shield ready!");

        if (type == SLAVE && setupSensor)
        {
            bme280::sensorInit();
        }
    }

    void sendRequest(void)
    {
        u8 message[] = {0xFF};
        debug::printDebug(debug::INFO, "Sending new requst...");

        loraRadio.write(message, 1);
    }

    // Only for SLAVE modules
    void sendResponse(bme280::SensorData_t *data)
    {
        u8 message[8]; // Payload

        // Split each 16-bit field into 2x 8-bit with bit masking
        message[0] = (data->temperature & 0xFF00) >> 8;
        message[1] = (data->temperature & 0x00FF);
        message[2] = (data->pressure & 0xFF00) >> 8;
        message[3] = (data->pressure & 0x00FF);

        debug::printDebug(debug::INFO, "Sending response with payload...");

        loraRadio.write(message, 8);
    }

    // Only for MASTER module
    u8 readResponse(lora::ReceivedData_t *data)
    {
        u8 message[8]; // Payload

        // Merge each 2x 8-bit fields into 1x 16-bit one, fix magnitudes
        data->temperature = (f32)(message[0] << 8 + message[1]) / 100;
        data->pressure = (f32)(message[2] << 8 + message[3]);

        memset(message, 0, 8);

        String temperatureMsg = "Temperature:" + String(data->temperature) + "\u00b0C";
        String pressureMsg = "Pressure:" + String(data->pressure) + "hPa";

        String formattedMessage[3] = {
            "Response received",
            temperatureMsg,
            pressureMsg,
        };

        for (u8 i = 0; i < (sizeof(formattedMessage) / sizeof(formattedMessage[0])); ++i)
        {
            debug::printDebug(debug::INFO, formattedMessage[i]);
        }

        return 0;
    }
}