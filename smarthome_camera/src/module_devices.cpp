#include "module_devices.h"
#include "module_server.h"
#include <ESP32Servo.h>
Servo myservo;
extern String FirmwareVersion;

void initLED()
{
    pinMode(LED_RED_NUM, OUTPUT);
    led_off();
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

void initServo()
{
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    myservo.setPeriodHertz(50);
    myservo.attach(SERVO_NUM, 1000, 2000);
    ServoStop();

    String Version = ReadData("Version");
    if (Version != "null") {
        FirmwareVersion = Version;
    }
}

void ServoStop()
{
    myservo.writeMicroseconds(1500);
}
void ServoLeft()
{
    myservo.writeMicroseconds(1800);
    delay(200);
    myservo.writeMicroseconds(1500);
}
void ServoRight()
{
    myservo.writeMicroseconds(1200);
    delay(200);
    myservo.writeMicroseconds(1500);
}