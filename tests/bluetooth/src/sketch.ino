/*
    Test for Bluetooth module
    HC-06 RS232

    Setup:
    http://playground.arduino.cc/uploads/Learning/bluetooth.gif
*/

#include <SoftwareSerial.h>

#define LEDpin 8

#define bluRX 2
#define bluTX 3

SoftwareSerial bluSerial(bluRX, bluTX);
char val;

void setup() {
    Serial.begin(9600);

    pinMode(13, OUTPUT);
    pinMode(LEDpin, OUTPUT);
    
    pinMode(bluRX, INPUT);
    pinMode(bluTX, OUTPUT);

    bluSerial.begin(9600);
    Serial.println("starting...");
}

void loop() {
    digitalWrite(13, LOW);
    digitalWrite(LEDpin, LOW);

    if(bluSerial.available()) {
      val = bluSerial.read();
    }
    
    if (Serial.available()) {
      Serial.println("! blutooth");
      Serial.println(val);
    }
    delay(1000);
}
