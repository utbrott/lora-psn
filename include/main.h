#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>

#include "globals.h"
#include "bme280.h"
#include "lora.h"
#include "debug.h"

#define INVERT(x) (x = (x + 1) % 2)

#define BOARD_BTN PC13
#define SLAVE_INTERRUPT_PIN PB3
#define I2C1_SDA PB9
#define I2C1_SCL PB8
#ifndef I2C_ADDR
#define I2C_ADDR 0x31
#endif
#define TIMEOUT_MS 1500
#define SLAVE_COUNT 3

/**
 * @brief Formatted data from sensor, ready for sending
 */
extern sensor::BufferData sensorBuffer;

/**
 * @brief Data received from the LoRa message
 */
extern lora::ReceivedData receivedData;

static const char slaveId[] = {0x01, 0x02, 0x03};
static const char dataId[] = {0x10, 0x20, 0x30};

/**
 * @brief Counter for total requests
 */
extern u8 totalRequests[SLAVE_COUNT];

/**
 * @brief Counter for failed requests
 */
extern u8 failedRequests[SLAVE_COUNT];

/**
 * @brief Data update request message sent by MASTER module and
 * received by SLAVE module. Contains board ID which should respond
 * and data ID which should be sent.
 */
extern u8 updateRequestMsg[1];

/**
 * @brief Holds data that is received through LoRa message
 */
extern u8 receivedMsg[64];

/**
 * @brief Time keeping variable
 * @note for MASTER - time between new requests in network
 * @note for SLAVE - time between taking new measurements
 */
extern u32 timer;

/**
 * @brief Flag to keep if next routine should be made
 * @note for MASTER - new requests to the network
 * @note for SLAVE - new measurement
 */
extern u8 next;

/**
 * @brief Button press interrupt handler
 * @note MASTER boards only
 */
extern void buttonPressHandler(void);

/**
 * @brief New data request interrupt handler
 * @note SLAVE boards only
 */
extern void newRequestHandler(void);

/**
 * @brief Function for handling data fetch subroutine
 */
extern void fetchSubroutineHandler(void);

/**
 * @brief Sends a data request with specifed code
 * @param requestCode Request code built from SLAVE Id and DATA Id
 */
extern void fetchData(u8 requestCode);

template <typename T, size_t size>
extern void logValues(const T (&array)[size]);

/**
 * @brief Data type key enumeration
 */
enum DataType
{
    TEMPERATURE,
    PRESSURE,
    HUMIDITY,
    FAILPERCENT,
};

/**
 * @brief Logs data from buffer
 * @param data pointer to data received by MASTER
 */
extern void logReceivedData(lora::ReceivedData *data);

template <typename T, size_t size>
extern void transmitPacket(const T (&array)[size], u8 type, f32 modifier = 1.0f);

/**
 * @brief Transmits data from buffer over I2C to Webserver module
 * @param data pointer to data received by MASTER
 */
extern void webserverTransmit(lora::ReceivedData *data);

extern void getFailedPercent(u8 (&totalReq)[3], u8 (&failReq)[3], f32 (&failPercent)[3]);

#endif /* MAIN_H */