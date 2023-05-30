#include "main.h"

// In case that build flag was not provided
#ifndef BOARD_TYPE
#define BOARD_TYPE lora::MASTER
#endif
#ifndef BOARD_ID
#define BOARD_ID 0x00
#endif
#ifndef SPREADFACTOR
#define SPREADFACTOR 7
#endif

// Set PERIOD_MS based on BOARD_ID
#if BOARD_ID == 0x00
#define PERIOD_MS 60000 // (milliseconds) between new requests
#else
#define PERIOD_MS 5000 // (milliseconds) between new measurements
#endif

// sensor::RawData_t sensorRaw = {0.0, 0.0, 0.0};
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
    lora::shieldInit(BOARD_TYPE, SPREADFACTOR, BOARD_ID);

    switch (BOARD_TYPE)
    {
    case lora::SLAVE:
    {
        sensor::init();
        sensor::readRaw();
        sensor::updateBuffer(&sensorBuffer);

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

        masterNewFetch_handler();
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
            sensor::readRaw();
            sensor::updateBuffer(&sensorBuffer);
            timer = millis();
            INVERT(next);
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
    debug::println(debug::INFO, "Found request: 0x" + String(updateRequestMsg[0], HEX));
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
    // i2cScan();
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
    if (receivedMsg[0] != requestCode)
    {
        debug::println(debug::ERR, "Fetch failed, response did not match.");
        return;
    }

    lora::readResponse(&receivedData, receivedMsg);
    delay(100); // 100ms blocking delay between requests
}

void logReceivedData(void)
{
    Serial.println();
    debug::println(debug::INFO, "Fetched data:");

    Serial.print("Temperature:\t");
    for (int i = 0; i < SLAVE_COUNT; ++i)
    {
        Serial.print(receivedData.temperature[i]);
        Serial.print("\t");
    }
    Serial.println();

    Serial.print("Pressure:\t");
    for (int i = 0; i < SLAVE_COUNT; ++i)
    {
        Serial.print(receivedData.pressure[i]);
        Serial.print("\t");
    }
    Serial.println();

    Serial.print("Humidity:\t");
    for (int i = 0; i < SLAVE_COUNT; ++i)
    {
        Serial.print(receivedData.humidity[i]);
        Serial.print("\t");
    }
    Serial.println();
}

void transmitData(lora::ReceivedData_t *data_ptr)
{
    debug::println(debug::INFO, "I2C Transmission");
    Wire.beginTransmission(I2C_ADDR);
    char str_buf[50];
    for (u8 i = 0; i < SLAVE_COUNT; ++i)
    {
        sprintf(str_buf, "%i\t", (u16)(data_ptr->temperature[i] * 100.0f));
        Wire.write(str_buf);
    }
    Wire.write("\n");

    for (u8 i = 0; i < SLAVE_COUNT; ++i)
    {
        sprintf(str_buf, "%i\t", (u16)(data_ptr->pressure[i]));
        Wire.write(str_buf);
    }
    Wire.write("\n");

    for (u8 i = 0; i < SLAVE_COUNT; ++i)
    {
        sprintf(str_buf, "%i\t", (u16)(data_ptr->humidity[i] * 100.0f));
        Wire.write(str_buf);
    }
    Wire.write("\n");

    Wire.endTransmission();
    debug::println(debug::INFO, "Done");
}