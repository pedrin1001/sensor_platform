#include <TimerOne.h>

int hrPin = 8;
int hrValue = 0;

void callback() {
    hrValue = digitalRead(hrPin);
    if (isnan(hrValue)) {
        Serial.println("Failed to read from HR202");
    } else {
        Serial.println(hrValue);
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
