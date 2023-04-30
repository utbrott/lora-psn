#include "main.h"

/**
 * @brief Local definition of board behaviour
 * @note `lora::MASTER` mode - requesting data, printing
 * acquired responses
 * @note `lora::SLAVE` mode - reading data from sensor, responding
 * to requests
 */
#define BOARD_TYPE lora::MASTER

/**
 * @brief Whether to initialize the BME280 sensor along the LoRa module
 * (boolean).
 * @note If this is set to `true` and there is not sensor, program will panic
 * and get halted until sensor is connected and board restarted.
 */
#define HAS_SENSOR true

bme280::SensorData_t sensorData = {0, 0};
lora::ReceivedData_t receivedData = {0.0, 0.0};
u8 requestMessage[1];
u8 receivedMessage[8];
u8 interruptState = 0;

void setup()
{
    Serial.begin(115200);
    lora::shieldInit(BOARD_TYPE, HAS_SENSOR);
    if (BOARD_TYPE == lora::MASTER)
    {
        attachInterrupt(digitalPinToInterrupt(BOARD_BTN), buttonClickInterrupt,
                        RISING);
    }
}

void loop()
{
    switch (BOARD_TYPE)
    {
    case lora::SLAVE:
        if (loraRadio.read(requestMessage))
        {
            Serial.println(String(requestMessage[0]));
            if (requestMessage[0] == 0xFF)
            {
                memset(requestMessage, 0, 1);
                debug::printDebug(debug::INFO, "Data request received");

                bme280::readData(&sensorData);
                lora::sendResponse(&sensorData);
            }
        }
        break;

    case lora::MASTER:
        if (interruptState)
        {
            lora::sendRequest();
            BOOL(interruptState);
        }

        if (loraRadio.read(receivedMessage) > 0)
        {
            lora::readResponse(&receivedData, receivedMessage);
        }
        break;
    }
}

void buttonClickInterrupt(void) { BOOL(interruptState); }