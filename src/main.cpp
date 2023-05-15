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
u8 requestMessage[1];
u8 receivedMessage[64];

u8 requestCode[] = {0x01, 0x02}; // SLAVE IDs to ask for data

u32 timer = 0;
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
                        dataRequestInterruptHandler, RISING);

        break;

    case lora::MASTER:
        attachInterrupt(digitalPinToInterrupt(BOARD_BTN),
                        buttonClickInterruptHandler, RISING);

        break;
    }
}

void loop()
{
    switch (BOARD_TYPE)
    {
    case lora::SLAVE:
        if (loraRadio.read(requestMessage))
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

        if (loraRadio.read(receivedMessage) > 0)
        {
            lora::readResponse(&receivedData, receivedMessage);
        }
        break;
    }

    if ((millis() - timer) >= PERIOD_MS)
    {
        INVERT(next);
    }
}

void buttonClickInterruptHandler(void) { INVERT(boardBtnPressed); }
void dataRequestInterruptHandler(void)
{
    if (BOARDID_MASK(requestMessage[0]) != BOARD_ID)
    {
        digitalWrite(LED_BUILTIN, 0);
        return;
    }

    lora::sendResponse(&sensorBuffer, DATAID_MASK(requestMessage[0]));
    digitalWrite(LED_BUILTIN, 0); // Turn off the LED when done, visual indicator
}