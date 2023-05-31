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

sensor::BufferData sensorBuffer = {0, 0, 0};

lora::ReceivedData receivedData = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
u8 updateRequestMsg[1]; // SLAVE only
u8 receivedMsg[64];     // MASTER only
u8 totalRequests[SLAVE_COUNT] = {0, 0, 0};
u8 failedRequests[SLAVE_COUNT] = {0, 0, 0};
f32 failedPercent[SLAVE_COUNT] = {0.0, 0.0, 0.0};

u8 requestCode[sizeof(slaveId) * sizeof(dataId)];

u32 timer = 0;
u8 next = 0;

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

    // Calculate percent of failed requests
    // for (size_t i; i < ARRAYSIZE(failedPercent); ++i)
    // {
    //     failedPercent[i] = ((f32)failedRequests[i] / (f32)totalRequests[i]) * 100.0f;
    // }

    logReceivedData(&receivedData);
    webserverTransmit(&receivedData);
}

void fetchDataUpdate(u8 requestCode)
{
    lora::sendRequest(requestCode);
    totalRequests[(BOARDID_MASK(requestCode)) - 1] += 1;

    // If MASTER waits 500ms for response, treat fetch as failed
    u32 timeout = millis();
    while (!(loraRadio.read(receivedMsg) > 0))
    {
        if ((millis() - timeout) >= TIMEOUT_MS)
        {
            timeout = millis();
            debug::println(debug::ERR, "Request 0x" + String(requestCode, HEX) + " failed: TIME OUT.");
            failedRequests[(BOARDID_MASK(requestCode)) - 1] += 1;
            return;
        }
    }

    // No timeout, check response code matches request code
    if (receivedMsg[0] != requestCode)
    {
        debug::println(debug::ERR, "Request 0x" + String(requestCode, HEX) + " failed: BAD RESPONSE");
        failedRequests[(BOARDID_MASK(requestCode)) - 1] += 1;
        return;
    }

    lora::readResponse(&receivedData, receivedMsg);
    delay(100); // 100ms blocking delay between requests
}

template <typename T, size_t size>
extern void logValues(const T (&array)[size])
{
    for (T item : array)
    {
        Serial.print(item);
        Serial.print("\t");
    }
    Serial.println();
}

void logReceivedData(lora::ReceivedData *data)
{
    Serial.println();
    debug::println(debug::INFO, "Fetched data:");

    Serial.print("Temperature:\t");
    logValues(data->temperature);

    Serial.print("Pressure:\t");
    logValues(data->pressure);

    Serial.print("Humidity:\t");
    logValues(data->humidity);

    Serial.println();
    debug::println(debug::INFO, "Requests statistics:");

    Serial.print("Total:\t\t");
    logValues(totalRequests);

    Serial.print("Failed:\t\t");
    logValues(failedRequests);

    for (size_t i = 0; i < ARRAYSIZE(totalRequests); ++i)
    {
        failedPercent[i] = ((f32)failedRequests[i] / (f32)totalRequests[i]) * 100.0f;
    }

    Serial.print("Failed%:\t");
    logValues(failedPercent);

    Serial.println();
    Serial.println();
}

void webserverTransmit(lora::ReceivedData *data)
{
    debug::println(debug::INFO, "Sending to webserver\n");
    Wire.beginTransmission(I2C_ADDR);
    char str_buf[50];
    for (u8 i = 0; i < SLAVE_COUNT; ++i)
    {
        sprintf(str_buf, "%i\t", (u16)(data->temperature[i] * 100.0f));
        Wire.write(str_buf);
    }
    Wire.write("\n");

    for (u8 i = 0; i < SLAVE_COUNT; ++i)
    {
        sprintf(str_buf, "%i\t", (u16)(data->pressure[i]));
        Wire.write(str_buf);
    }
    Wire.write("\n");

    for (u8 i = 0; i < SLAVE_COUNT; ++i)
    {
        sprintf(str_buf, "%i\t", (u16)(data->humidity[i] * 100.0f));
        Wire.write(str_buf);
    }
    Wire.write("\n");
    Wire.endTransmission();
}