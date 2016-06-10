#include <TimerOne.h>

int mq7Pin = A0;
int mq7Value = 0;

void callback() {
    mq7Value = analogRead(mq7Pin);
    if (isnan(mq7Value)) {
        Serial.println("Failed to read from MQ7");
    } else {
        Serial.println(mq7Value);
    }
}

void setup()
{
    Serial.begin(9600);
    Timer1.initialize(1000000);
    Timer1.attachInterrupt(callback);
}

void loop()
{
}
