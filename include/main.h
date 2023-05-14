#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>

#include "globals.h"
#include "bme280.h"
#include "lora.h"
#include "debug.h"

#define INVERT(x) (x = (x + 1) % 2)

#define REQ_VALUE(ID, REQ) ((ID) + (REQ))

#define BOARD_BTN PC13
#define SLAVE_INTERRUPT_PIN PB3

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
 */
extern void Hwi_ButtonClick(void);

/**
 * @brief New data request interrupt handler
 */
extern void Swi_DataRequest(void);

/**
 * @brief Get the Data Id
 *
 * @param request Request value from LoRa message
 * @return u8 Data Id
 */
extern u8 getDataId(u8 request);

/**
 * @brief Get the Board Id
 *
 * @param request Request value from LoRa message
 * @return u8
 */
extern u8 getBoardId(u8 request);

#endif /* MAIN_H */