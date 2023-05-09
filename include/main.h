#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>

#include "bme280.h"
#include "debug.h"
#include "globals.h"
#include "lora.h"

#define BOOL(x) (x = (x + 1) % 2)

#define BOARD_BTN PC13
#define BOARD_LED PA5

/**
 * @brief Data read by the BME280 sensor
 */
extern bme280::SensorData_t sensorData;

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
extern u8 interruptState;

/**
 * @brief What actions should be done when board button is pressed
 */
extern void buttonClickInterrupt(void);

#endif /* MAIN_H */