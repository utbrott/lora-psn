#ifndef LORA_H
#define LORA_H

#include <Arduino.h>
#include <Wire.h>
#include <LoRaRadio.h>

#include "globals.h"
#include "bme280.h"

#define UART1_RX PA10
#define UART1_TX PA9
#define ARRAYSIZE(n) (sizeof(n) / sizeof(n[0]))
#define DEFUALT_BOARD_ID 0x00

namespace lora
{
    /**
     * @brief   Behaviour of the board enumeration
     */
    typedef enum
    {
        SLAVE,
        MASTER,
    } BoardType_t;

    /**
     * @brief   Structure of data received through LoRa communication
     */
    typedef struct
    {
        f32 temperature[3];
        f32 pressure[3];
        f32 humidity[3];
    } ReceivedData_t;

    enum dataId : const char
    {
        TEMPERATURE = 0x10,
        PRESSURE = 0x20,
        HUMIDITY = 0x30,
    };

    /**
     * @brief   Initialize LoRa shield
     * @param   boardType: Board type, values from @ref BoardType_t
     * @param   boardId: Board ID, hex value from 0x00 to 0x0f
     * @note    Also performs BME280 sensor init if board is specified as SLAVE
     */
    void shieldInit(BoardType_t boardType, u8 boardId = DEFUALT_BOARD_ID);

    /**
     * @brief   Sends data request
     * @note    MASTER boards only
     */
    void sendRequest(u8 message);

    /**
     * @brief   Sends response with sensor data (if on board)
     * @param   *data: data from the sensor, if connected
     * otherwise defaults to empty array
     */
    void sendResponse(sensor::BufferData_t *buffer, u8 reqCode);

    /**
     * @brief   Read response from LoRa connection
     * @param   message: Message received through LoRa
     */
    void readResponse(ReceivedData_t *receivedData, u8 message[]);
}

#endif /* LORA_H */
