#include "main.h"

// In case that build flag was not provided
#ifndef BOARD_TYPE
#define BOARD_TYPE lora::MASTER
#endif
#ifndef BOARD_ID
#define BOARD_ID 0x00
#endif
#ifndef SPREADFACTOR
#define SPREADFACTOR 12
#endif

// Set PERIOD_MS based on BOARD_ID
#if BOARD_ID == 0x00
#define PERIOD_MS 60000 // (milliseconds) between new requests
#else
#define PERIOD_MS 5000 // (milliseconds) between new measurements
#endif

sensor::BufferData sensorBuffer = {0, 0, 0};

lora::ReceivedData receivedData = {
    {0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0}};
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
                        newRequestHandler, RISING);

        break;
    }

    case lora::MASTER:
    {
        u8 codeItr = 0;
        for (u8 _dId : dataId)
        {
            for (u8 _sId : slaveId)
            {
                requestCode[codeItr] = _dId + _sId;
                ++codeItr;
            }
        }

        Wire.setSDA(I2C1_SDA);
        Wire.setSCL(I2C1_SCL);
        Wire.begin();
        attachInterrupt(digitalPinToInterrupt(BOARD_BTN), buttonPressHandler,
                        RISING);

        fetchSubroutineHandler();
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
            debug::println(debug::INFO, "Measuring");
            sensor::readRaw();
            sensor::updateBuffer(&sensorBuffer);
            timer = millis();
            INVERT(next);
            debug::println(debug::INFO, "Measure done");
        }
        break;

    case lora::MASTER:
        if (next)
        {
            timer = millis();
            fetchSubroutineHandler();
            INVERT(next);
        }
        break;
    }

    if ((millis() - timer) >= PERIOD_MS)
    {
        INVERT(next);
    }
}

void buttonPressHandler(void) { fetchSubroutineHandler(); }

void newRequestHandler(void)
{
    if (BOARDID_MASK(updateRequestMsg[0]) != BOARD_ID)
    {
        digitalWrite(LED_BUILTIN, 0);
        return;
    }

    lora::sendResponse(&sensorBuffer, updateRequestMsg[0]);
    digitalWrite(LED_BUILTIN, 0); // Turn off the LED when done, visual indicator
}

void fetchSubroutineHandler(void)
{
    for (u8 code : requestCode)
    {
        fetchData(code);
    }

    logReceivedData(&receivedData);
    webserverTransmit(&receivedData);
}

void fetchData(u8 requestCode)
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
            debug::println(debug::ERR, "Request 0x" + String(requestCode, HEX) +
                                           " failed: TIME OUT.");
            failedRequests[(BOARDID_MASK(requestCode)) - 1] += 1;
            return;
        }
    }

    // No timeout, check response code matches request code
    if (receivedMsg[0] != requestCode)
    {
        debug::println(debug::ERR, "Request 0x" + String(requestCode, HEX) +
                                       " failed: BAD RESPONSE");
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
    getFailedPercent(totalRequests, failedRequests, failedPercent);
    Serial.print("Failed%:\t");
    logValues(failedPercent);

    Serial.println();
}

template <typename T, size_t size>
void transmitPacket(const T (&array)[size], u8 typeKey, f32 modifier)
{
    char packet[50];
    for (T item : array)
    {
        sprintf(packet, "%i:%i&", typeKey, (u16)(item * modifier));
        Wire.write(packet);
    }
}

void webserverTransmit(lora::ReceivedData *data)
{
    debug::println(debug::INFO, "Sending to webserver");
    Wire.beginTransmission(I2C_ADDR);

    transmitPacket(data->temperature, TEMPERATURE, 100.0f);
    Serial.println("Temperature");
    transmitPacket(data->pressure, PRESSURE);
    Serial.println("Pressure");
    transmitPacket(data->humidity, HUMIDITY, 100.0f);
    Serial.println("Humidity");
    getFailedPercent(totalRequests, failedRequests, failedPercent);
    transmitPacket(failedPercent, FAILPERCENT, 100.0f);
    Serial.println("Failed%");

    Wire.endTransmission();
}

void getFailedPercent(u8 (&totalReq)[3], u8 (&failReq)[3],
                      f32 (&failPercent)[3])
{
    for (u8 i = 0; i < sizeof(totalReq); ++i)
    {
        failedPercent[i] = ((f32)failReq[i] / (f32)totalReq[i]) * 100.0f;
    }
}
