#include "main.h"

// In case that build flag was not provided
#ifndef BOARD_TYPE
#define BOARD_TYPE lora::MASTER
#endif
#ifndef BOARD_ID
#define BOARD_ID 0x00
#endif

// Set PERIOD_MS based on chosed board
#if BOARD_ID == 0x00
#define PERIOD_MS 60000 // (milliseconds) between new requests
#else
#define PERIOD_MS 5000 // (milliseconds) between new measurements
#endif

sensor::RawData_t sensorRaw = {0.0, 0.0, 0.0};
sensor::BufferData_t sensorBuffer = {0, 0, 0};

lora::ReceivedData_t receivedData = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
u8 updateRequestMsg[1]; // SLAVE only
u8 receivedMsg[64];     // MASTER only

u8 requestCode[sizeof(slaveId) * sizeof(dataId)];

u32 timer = 0;
u8 next = 0;

u8 boardBtnPressed = 0;
u8 newDataRequest = 0;

void transmitData(lora::ReceivedData_t *data_ptr);
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
        sensor::updateBuffer(&sensorBuffer, &sensorRaw);

        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, 0);
        attachInterrupt(digitalPinToInterrupt(SLAVE_INTERRUPT_PIN),
                        updateRequest_handler, RISING);

        break;
    }

    case lora::MASTER:
    {
        u8 codeItr = 0;
        for (u8 i = 0; i < sizeof(dataId); ++i)
        {
            for (u8 j = 0; j < sizeof(slaveId); ++j)
            {
                requestCode[codeItr] = slaveId[j] + dataId[i];
                ++codeItr;
            }
        }

        Wire.setSDA(I2C1_SDA);
        Wire.setSCL(I2C1_SCL);
        Wire.begin();
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
        if (next)
        {
            timer = millis();
            masterNewFetch_handler();

            INVERT(next);
        }
        break;
    }

    if ((millis() - timer) >= PERIOD_MS)
    {
        INVERT(next);
    }
}

void buttonPress_handler(void)
{
    masterNewFetch_handler();
}

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

void masterNewFetch_handler(void)
{
    for (u8 itr = 0; itr < sizeof(requestCode); ++itr)
    {
        fetchDataUpdate(requestCode[itr]);
    }

    logReceivedData();
    transmitData(&receivedData);
}

void fetchDataUpdate(u8 requestCode)
{
    lora::sendRequest(requestCode);

    // If MASTER waits 500ms for response, treat fetch as failed
    u32 timeout = millis();
    while (!(loraRadio.read(receivedMsg) > 0))
    {
        if ((millis() - timeout) >= TIMEOUT_MS)
        {
            timeout = millis();
            debug::println(debug::ERR, "Request 0x" + String(requestCode, HEX) + " timed out.");
            return;
        }
    }

    // No timeout, check response code matches request code
    if (receivedMsg[2] != requestCode)
    {
        debug::println(debug::ERR, "Fetch failed, response did not match.");
        return;
    }

    lora::readResponse(&receivedData, receivedMsg);
}

void logReceivedData(void)
{
    Serial.println("\n");
    debug::println(debug::INFO, "Fetched data:");

    Serial.print("Temperature:\t");
    for (int i = 0; i < 3; ++i)
    {
        Serial.print(receivedData.temperature[i]);
        Serial.print("\t");
    }
    Serial.println();

    Serial.print("Pressure:\t");
    for (int i = 0; i < 3; ++i)
    {
        Serial.print(receivedData.pressure[i]);
        Serial.print("\t");
    }
    Serial.println();

    Serial.print("Humidity:\t");
    for (int i = 0; i < 3; ++i)
    {
        Serial.print(receivedData.humidity[i]);
        Serial.print("\t");
    }
    Serial.println();
}
void transmitData(lora::ReceivedData_t *data_ptr)
{
    Wire.beginTransmission(I2C_ADDR);
    for (u8 i = 0; i < 3; ++i)
    {
        Wire.write((u16)data_ptr->temperature[i]);
    }
    Wire.write("\n");

    for (u8 i = 0; i < 3; ++i)
    {
        Wire.write((u16)data_ptr->pressure[i]);
    }
    Wire.write("\n");
    for (u8 i = 0; i < 3; ++i)
    {
        Wire.write((u16)data_ptr->humidity[i]);
    }
    Wire.write("\n");
    Wire.endTransmission();
}