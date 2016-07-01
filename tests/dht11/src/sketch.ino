#include <DHT.h>

#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(9600);
    dht.begin();
}

void loop() {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (isnan(t) || isnan(h)) {
        Serial.println("Failed to read from DHT");
    }
    else {
        Serial.print("Humi: ");
        Serial.println(h);
        Serial.print("Temp: ");
        Serial.println(t);
        delay(1000);
    }
}
