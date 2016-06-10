#include <TimerOne.h>

int mq2Pin = A0;
int mq2Value = 0;

void callback() {
    mq2Value = analogRead(mq2Pin);
    if (isnan(mq2Value)) {
        Serial.println("Failed to read from MQ2");
    } else {
        Serial.println(mq2Value);
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
