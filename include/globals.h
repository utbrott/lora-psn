#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <Arduino.h>

#define DATAID_MASK(req) (req & 0xf0)
#define BOARDID_MASK(req) (req & 0x0f)
#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))

/**
 * @defgroup    Global_Typedef
 * @brief       Global typedefinitons
 */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef float f32;
typedef double f64;

#endif /* _GLOBALS_H */