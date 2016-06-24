/*
    Test for Bluetooth module
    HC-06 RS232

    Setup:
    http://playground.arduino.cc/uploads/Learning/bluetooth.gif
*/

#include <SoftwareSerial.h>

#define LEDpin 13

#define BluRX 0
#define BluTX 1

SoftwareSerial bluSerial(BluRX, BluTX);

void setup() {
    Serial.begin(9600);

    pinMode(BluRX, INPUT);
    pinMode(BluTX, OUTPUT);

    setupBlueToothConnection();
    
    pinMode(LEDpin, OUTPUT);
}

void loop() {
    /* digitalWrite(13, LOW); */
    digitalWrite(LEDpin, HIGH);
    delay(2000);
    digitalWrite(LEDpin, LOW);
    delay(2000);
    
    /* char val; */
    /* while(1) { */
    /*     if (Serial.available()) { */
    /*         if (bluSerial.available()) { */
    /*             val = bluSerial.read(); */
    /*             Serial.print(val); */
    /*         } */
    /*     } */
    /* } */
    /* if (val == '1') { */
    /*     digitalWrite(LEDpin, HIGH); */
    /* } */
    /* if (val == '0') { */
    /*     digitalWrite(LEDpin, LOW); */
    /* } */
}

void setupBlueToothConnection()
{
 bluSerial.begin(38400); //Set BluetoothBee BaudRate to default baud rate 38400
 bluSerial.print("\r\n+STWMOD=0\r\n"); //set the bluetooth work in slave mode
 bluSerial.print("\r\n+STNA=SeeedBTSlave\r\n"); //set the bluetooth name as "SeeedBTSlave"
 bluSerial.print("\r\n+STOAUT=1\r\n"); // Permit Paired device to connect me
 bluSerial.print("\r\n+STAUTO=0\r\n"); // Auto-connection should be forbidden here
 delay(2000); // This delay is required.
 bluSerial.print("\r\n+INQ=1\r\n"); //make the slave bluetooth inquirable 
 Serial.println("The slave bluetooth is inquirable!");
 delay(2000); // This delay is required.
 bluSerial.flush();
}
