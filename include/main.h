#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>

#include "globals.h"
#include "bme280.h"
#include "lora.h"
#include "debug.h"

#define BOOL(x) (x = (x + 1) % 2)
#define REQ_VALUE(ID, REQ) ((ID) + (REQ))

#define BOARD_BTN PC13
#define BOARD_LED PA5

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

/**
 * @brief Request message sent by MASTER module and received by SLAVE module
 */
extern u8 requestMessage[1];

/**
 * @brief Holds data that is received through LoRa message
 */
extern u8 receivedMessage[64];

/**
 * @brief Flag to hold current button interrupt state value
 */
extern u8 boardBtnPressed;

/**
 * @brief Flag to hold current button interrupt state value
 */
extern u8 newDataRequest;

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
extern bool next;

/**
 * @brief Button press interrupt handler
 */
extern void Hwi_ButtonClick(void);

/**
 * @brief New data request interrupt handler
 */
extern void Swi_DataRequest(void);

#endif /* MAIN_H */