
int sensorValue;

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    sensorValue = analogRead(0);       // read analog input pin 0
    Serial.print(sensorValue, DEC);  // prints the value read
    Serial.println("ppm");
    delay(100);
}
