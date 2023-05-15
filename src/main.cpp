#include "main.h"

#define BOARD_TYPE lora::MASTER
/**
 * @brief Sets board ID
 * @note MASTER: 0x00,
 * @note SLAVE: 0x01 - 0x0f
 */
#define BOARD_ID 0x00

#if BOARD_ID == 0x00
#define PERIOD_MS 60000 // (milliseconds) between new requests
#else
#define PERIOD_MS 5000 // (milliseconds) between new measurements
#endif

sensor::RawData_t sensorRaw = {0.0, 0.0, 0.0};
sensor::BufferData_t sensorBuffer = {0, 0, 0};

lora::ReceivedData_t receivedData = {0.0, 0.0, 0.0};
u8 updateRequestMsg[1]; // SLAVE only
u8 receivedMsg[64];     // MASTER only

u8 requestCode[];

u32 timer = 0;
u32 timeout = 0;
u8 next = 0;

u8 boardBtnPressed = 0;
u8 newDataRequest = 0;

void setup()
{
    Serial.begin(115200);
    lora::shieldInit(BOARD_TYPE);

    switch (BOARD_TYPE)
    {
    case lora::SLAVE:
        sensor::init();
        sensor::readRaw(&sensorRaw);
        sensor::updateBuffer(&sensorBuffer, &sensorRaw);

        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, 0);
        attachInterrupt(digitalPinToInterrupt(SLAVE_INTERRUPT_PIN),
                        updateRequest_handler, RISING);

        break;

    case lora::MASTER:
    {
        u8 codeItr = 0;
        for (u8 i = 0; i < sizeof(slaveId); ++i)
        {
            for (u8 j = 0; j < sizeof(dataId); ++j)
            {
                requestCode[codeItr] = slaveId[i] + dataId[j];
                ++codeItr;
            }
        }

        attachInterrupt(digitalPinToInterrupt(BOARD_BTN),
                        buttonPress_handler, RISING);

        break;
    }
    }
}

void loop()
{
    switch (BOARD_TYPE)
    {
    case lora::SLAVE:
        if (loraRadio.read(updateRequestMsg))
        {
            // This will trigger an interrupt
            digitalWrite(LED_BUILTIN, 1);
        }

        sensor::RawData_t measured;
        if (next)
        {
            sensor::readRaw(&measured);
            timer = millis();
            INVERT(next);
        }

        if (sensor::compareValues(&sensorRaw, &measured))
        {
            sensor::updateBuffer(&sensorBuffer, &sensorRaw);
            debug::println(debug::INFO, "Data changed, updating buffer");
        }
        break;

    case lora::MASTER:
        if (boardBtnPressed)
        {
            lora::sendRequest(0x11);
            INVERT(boardBtnPressed);
        }

        if (next)
        {
            timer = millis();
            INVERT(next);
        }

        if (loraRadio.read(receivedMsg) > 0)
        {
            lora::readResponse(&receivedData, receivedMsg);
        }
        break;
    }

    if ((millis() - timer) >= PERIOD_MS)
    {
        INVERT(next);
    }
}

void buttonPress_handler(void) { INVERT(boardBtnPressed); }
void updateRequest_handler(void)
{
    if (BOARDID_MASK(updateRequestMsg[0]) != BOARD_ID)
    {
        digitalWrite(LED_BUILTIN, 0);
        return;
    }

    lora::sendResponse(&sensorBuffer, updateRequestMsg[0]);
    digitalWrite(LED_BUILTIN, 0); // Turn off the LED when done, visual indicator
}

bool fetchDataUpdate(u8 requestCode)
{
    lora::sendRequest(requestCode);
    while (!(loraRadio.read(receivedMsg) > 0))
    {
        timeout = millis();
        // If MASTER waits 500ms for response, treat fetch as failed
        if ((millis() - timeout) >= TIMEOUT_MS)
        {
            timeout = 0;
            return false;
        }
    }

    u8 responseCode = receivedMsg[3];
}