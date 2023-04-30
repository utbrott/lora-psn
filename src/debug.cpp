#include "debug.h"

void debug::printDebug(debug::MsgType_t type, String msg)
{
    String msgPrefix = type ? "[ERR] " : "[INFO] ";
    Serial.println(msgPrefix + msg);
}