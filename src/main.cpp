#include "main.h"

#define BOARD_TYPE lora::SLAVE
/**
 * @brief Sets board ID
 * @note MASTER: 0x00,
 * @note SLAVE: 0x01 - 0x0f
 */
#define BOARD_ID 0x01

#if BOARD_ID == 0x00
#define PERIOD_MS 60000 // (milliseconds) between new requests
#else
#define PERIOD_MS 5000 // (milliseconds) between new measurements
#endif

sensor::RawData_t sensorRaw = {0.0, 0.0, 0.0};
sensor::BufferData_t sensorBuffer = {0, 0, 0};

lora::ReceivedData_t receivedData = {0.0, 0.0, 0.0};
u8 requestMessage[1];
u8 receivedMessage[64];

u8 requestCode[] = {0x01, 0x02}; // SLAVE IDs to ask for data

u32 timer = 0;
bool next = false;

u8 boardBtnPressed = 0;
u8 newDataRequest = 0;

void setup()
{
    Serial.begin(115200);
    lora::shieldInit(BOARD_TYPE);

    switch (BOARD_TYPE)
    {
    case lora::SLAVE:
    {
        sensor::init();
        sensor::readRaw(&sensorRaw);
        pinMode(BOARD_LED, OUTPUT);
        attachInterrupt(digitalPinToInterrupt(BOARD_LED), Swi_DataRequest,
                        RISING);
        break;
    }

    case lora::MASTER:
        attachInterrupt(digitalPinToInterrupt(BOARD_BTN), Hwi_ButtonClick,
                        RISING);
        break;
    }
}

void loop()
{
    switch (BOARD_TYPE)
    {
    case lora::SLAVE:
    {
        sensor::RawData_t measured;
        bool didChange = false;
        if (next)
        {
            sensor::readRaw(&measured);
            didChange = sensor::compareValues(&sensorRaw, &measured);
            timer = millis();
            next = false;
        }

        if (didChange)
        {
            debug::println(debug::INFO, "Data changed, updating buffer");
            sensor::updateBuffer(&sensorBuffer, &sensorRaw);
            didChange = false;
        }

        // if (loraRadio.read(requestMessage))
        // {
        //     debug::println(debug::INFO, "New request: 0x" + (String(requestMessage[0], HEX)));
        //     if (memcmp(requestMessage, requestCode, 1) == 0)
        //     {
        //         memset(requestMessage, 0, 1);
        //         debug::println(debug::INFO, "Data request received");

        //         // lora::sendResponse(&sensorBuffer);
        //     }
        // }
        break;
    }

    case lora::MASTER:

        if (boardBtnPressed)
        {
            lora::sendRequest();
            BOOL(boardBtnPressed);
        }

        if (next)
        {
            timer = millis();
            next = false;
        }

        if (loraRadio.read(receivedMessage) > 0)
        {
            lora::readResponse(&receivedData, receivedMessage);
        }
        break;
    }

    if ((millis() - timer) >= PERIOD_MS)
    {
        next = true;
    }
}

void Hwi_ButtonClick(void) { BOOL(boardBtnPressed); }
void Swi_DataRequest(void) { BOOL(newDataRequest); }