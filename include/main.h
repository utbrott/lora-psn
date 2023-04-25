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
 * @brief Defines how the board behaves
 */
extern lora::BoardType_t boardType;

/**
 * @brief Flag to hold current button interrupt state value
 */
extern u8 interruptState;

/**
 * @brief What actions should be done when board button is pressed
 */
extern void buttonClickInterrupt(void);

#endif /* MAIN_H */