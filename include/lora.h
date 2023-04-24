#ifndef LORA_H
#define LORA_H

#include <Arduino.h>
#include <Wire.h>
#include <LoRaRadio.h>

#include "globals.h"
#include "bme280_sensor.h"

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
        f32 temperature;
        f32 pressure;
    } ReceivedData_t;

    /**
     * @brief   Initialize LoRa shield
     * @param   boardType: Board type, values from @ref BoardType_t
     * @note    Performs BME280 sensor init if board is specified as SLAVE
     *          and setup flag is `true`
     */
    void shieldInit(BoardType_t boardType, boolean setupSensor);

    /**
     * @brief   Sends data request
     * @note    MASTER boards only
     */
    void sendRequest(void);

    /**
     * @brief   Sends response with sensor data (if on board)
     * @param   *data: data from the sensor, if connected
     * otherwise defaults to empty array
     */
    void sendResponse(bme280::SensorData_t *data);

    /**
     * @brief   Read response from LoRa connection
     * @param   *data: data received through LoRa
     * @retval  Received data array
     */
    u8 readResponse(ReceivedData_t *data);
}

#endif /* LORA_H */