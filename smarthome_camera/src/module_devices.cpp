#include "module_devices.h"

void initLED()
{
    pinMode(LED_RED_NUM, OUTPUT);
    digitalWrite(LED_RED_NUM, LOW);
}

void led_off()
{
    digitalWrite(LED_RED_NUM, LOW);
}

void led_on()
{
    digitalWrite(LED_RED_NUM, HIGH);
}

void blinkLED(int n, int t)
{
    for (int i = 0; i < 2 * n; i++) {
        digitalWrite(LED_RED_NUM, !digitalRead(LED_RED_NUM));
        delay(t);
    }
}
