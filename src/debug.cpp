#include "debug.h"

namespace debug
{
    void println(MsgType_t type, String msg)
    {
        String msgPrefix = type ? "[ERR] " : "[INFO] ";
        Serial.println(msgPrefix + msg);
    }
}
