#include <DHT.h>
#include <TimerOne.h>

#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float h = 0;
float t = 0;

int mq7Pin = A0;
int mq7Value = 0;

int mq2Pin = A2;
int mq2Value = 0;

void callback() {
    mq7Value = analogRead(mq7Pin);
    if (isnan(mq7Value)) {
        Serial.println("Failed to read from MQ7");
    } else {
        Serial.print("MQ7: ");
        Serial.print(mq7Value);
    }
    mq2Value = analogRead(mq2Pin);
    if (isnan(mq2Value)) {
        Serial.println("Failed to read from MQ2");
    } else {
        Serial.print("  MQ2: ");
        Serial.print(mq2Value);
    }
    Serial.print("  Humi: ");
    Serial.print(h);
    Serial.print("  Temp: ");
    Serial.print(t);
    Serial.print("  HeatIdx: ");
    Serial.print(dht.computeHeatIndex(t, h, false));
    Serial.println();
}

void setup() {
    Serial.begin(9600);
    dht.begin();
    Timer1.initialize(1000000);
    Timer1.attachInterrupt(callback);
}

void loop() {
    h = dht.readHumidity();
    t = dht.readTemperature();
    if (isnan(t) || isnan(h)) {
        Serial.println("Failed to read from DHT");
    }
}
