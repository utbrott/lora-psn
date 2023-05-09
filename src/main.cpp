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
 * @brief Local definition of SLAVE board ID, for network access
 * @note Set a number value from 0x00 to 0xff to identify the board
 */
#define SLAVE_ID 0x01

/**
 * @brief How often (milliseconds) MASTER module should ask SLAVE modules for
 * data updates
 */
#define REQUEST_PERDIOD_MS 60000

bme280::SensorData_t sensorData = {0, 0, 0};
lora::ReceivedData_t receivedData = {0.0, 0.0, 0.0};
u8 requestMessage[1];
u8 receivedMessage[64];
u8 interruptState = 0;

u8 requestCode[] = {0x01, 0x02}; // SLAVE IDs to ask for data
bool nextRequest = true;         // Next request flag
int requestTimer = 0;            // Request period timer

void setup()
{
    Serial.begin(115200);
    lora::shieldInit(BOARD_TYPE);
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
            debug::printDebug(debug::INFO, "New request: 0x" + (String(requestMessage[0], HEX)));
            if (memcmp(requestMessage, requestCode, 1) == 0)
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

        if (nextRequest)
        {
            BOOL(nextRequest);
        }

        if (loraRadio.read(receivedMessage) > 0)
        {
            lora::readResponse(&receivedData, receivedMessage);
        }

        if ((millis() - requestTimer) >= REQUEST_PERDIOD_MS)
        {
            BOOL(nextRequest);
        }
        break;
    }
}

void buttonClickInterrupt(void) { BOOL(interruptState); }