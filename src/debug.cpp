#include "debug.h"

void debug::println(debug::MsgType_t type, String msg)
{
    String msgPrefix = type ? "[ERR] " : "[INFO] ";
    Serial.println(msgPrefix + msg);
}