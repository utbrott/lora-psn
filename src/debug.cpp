#include "debug.h"

void debug::printDebug(debug::MsgType_t type, const char *msg)
{
    String msgPrefix = type ? "[ERR]" : "[INFO]";
    Serial.println(msgPrefix + String(msg));
}