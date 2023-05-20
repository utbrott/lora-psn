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
#define TIMEOUT_MS 1500

/**
 * @brief Raw data read by the BME280 sensor
 */
extern sensor::RawData_t sensorRaw;

/**
 * @brief Formatted data from sensor, ready for sending
 */
extern sensor::BufferData_t sensorBuffer;

/**
 * @brief Data received from the LoRa message
 */
extern lora::ReceivedData_t receivedData;

static const char slaveId[] = {0x01, 0x02, 0x03};
static const char dataId[] = {0x10, 0x20, 0x30};

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
 * @brief Flag to hold current button interrupt state value
 */
extern u8 boardBtnPressed;

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
extern void buttonPress_handler(void);

/**
 * @brief New data request interrupt handler
 * @note SLAVE boards only
 */
extern void updateRequest_handler(void);

/**
 * @brief Handler for making new fetch with timer or button press interrupt
 */
extern void masterNewFetch_handler(void);

/**
 * @brief Performs data update routine with specifed request code
 * @param requestCode Request code built from SLAVE Id and DATA Id
 */
extern void fetchDataUpdate(u8 requestCode);

// Temporary
extern void logReceivedData(void);

