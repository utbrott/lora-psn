#include "main.h"

/**
 * @brief Local definition of board behaviour
 * @note `lora::MASTER` mode - requesting data, printing
 * acquired responses
 * @note `lora::SLAVE` mode - reading data from sensor, responding
 * to requests
 */
#define BOARD_TYPE lora::SLAVE

/**
 * @brief Whether to initialize the BME280 sensor along the LoRa module
 * (boolean).
 * @note If this is set to `true` and there is not sensor, program will panic
 * and get halted until sensor is connected and board restarted.
 */
#define HAS_SENSOR true

lora::BoardType_t boardType = BOARD_TYPE;
u8 requestMessage[1];
u8 receivedMessage[8];

void setup()
{
    // Init Serial, used for debug messages
    Serial.begin(115200);

    // Init the LoRa shield
    lora::shieldInit(boardType, HAS_SENSOR);

    // Attach button interrupt if board is MASTER
    if (boardType == lora::MASTER)
    {
        attachInterrupt(digitalPinToInterrupt(BOARD_BTN), buttonClickInterrupt,
                        RISING);
    }
}

void loop()
{
    switch (boardType)
    {
    case lora::SLAVE:
        if (loraRadio.read(requestMessage) == 0x04)
        {
            memset(requestMessage, 0, 1);
            debug::printDebug(debug::INFO, "Data request received");

            lora::sendResponse(&bme280::readData());
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
            lora::readResponse(receivedMessage);
        }
        break;
    }
}

void buttonClickInterrupt(void) { BOOL(interruptState); }