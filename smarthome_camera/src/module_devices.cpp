#include "module_devices.h"
#include "module_server.h"
#include <ESP32Servo.h>
Servo myservo;
int servoPos = 0;

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

void initServo()
{
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    myservo.setPeriodHertz(50); // standard 50 hz servo
    myservo.attach(SERVO_NUM, 1000, 2000);
    int value = ReadintData("pos");
    if (value != 1000) {
        servoPos = value;
        myservo.write(servoPos);
    }
}

void ServoLeft()
{
    int value = myservo.read();
    value -= 30;
    if (value <= 0) {
        value = 0;
    }
    if (value >= 180) {
        value = 180;
    }

    for (; servoPos > value; servoPos -= 1) {
        myservo.write(servoPos);
        delay(15);
    }
    StoreintData("pos", servoPos);
}
void ServoRight()
{
    int value = myservo.read();
    value += 30;
    if (value <= 0) {
        value = 0;
    }
    if (value >= 180) {
        value = 180;
    }
    for (; servoPos < value; servoPos += 1) {
        myservo.write(servoPos);
        delay(15);
    }
    StoreintData("pos", servoPos);
}