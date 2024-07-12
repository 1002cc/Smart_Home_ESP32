#include "module_devices.h"

void initLED()
{
    pinMode(LED_RED_NUM, OUTPUT);
    digitalWrite(LED_RED_NUM, LOW);
}
