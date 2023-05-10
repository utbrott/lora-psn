#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

namespace debug
{
    /**
     * @brief Debug message enumeration
     */
    typedef enum
    {
        INFO,
        ERR,
    } MsgType_t;

    /**
     * @brief Prints out a debug message over Serial
     *
     * @param type: Debug message type @ref MsgType_t
     * @param msg: Message string
     */
    void println(MsgType_t type, String msg);
}

#endif /* DEBUG_H */