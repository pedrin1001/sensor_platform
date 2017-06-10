/*
	SFE_BMP180.cpp
	Bosch BMP180 pressure sensor library for the Arduino microcontroller
	Mike Grusin, SparkFun Electronics

	Uses floating-point equations from the Weather Station Data Logger project
	http://wmrx00.sourceforge.net/
	http://wmrx00.sourceforge.net/Arduino/BMP085-Calcs.pdf

	Forked from BMP085 library by M.Grusin

	version 1.0 2013/09/20 initial version
	Verison 1.1.2 - Updated for Arduino 1.6.4 5/2015

	Our example code uses the "beerware" license. You can do anything
	you like with this code. No really, anything. If you find it useful,
	buy me a (root) beer someday.
*/

#include "SFE_BMP180.h"
#include <Wire.h>
#include <stdio.h>
#include <math.h>


SFE_BMP180::SFE_BMP180()
// Base library type
{
}


char SFE_BMP180::begin()
// Initialize library for subsequent pressure measurements
{
	// Start up the Arduino's "wire" (I2C) library:

	// Wire.begin();

	// The BMP180 includes factory calibration data stored on the device.
	// Each device has different numbers, these must be retrieved and
	// used in the calculations when taking pressure measurements.

	#ifndef WRITE_EEPROM
	if (eeprom.readInt(EEPROM_CHECK) == 42) {
		uint8_t address = 0;
		// all commented out varialbles arejust used as helpers for calibration
		// but also saved on eeprom, so we skip them
		// AC1 = address;
		address += sizeof(int);
		// AC2 = address;
		address += sizeof(int);
		// AC3 = address;
		address += sizeof(int);
		// AC4 = address;
		address += sizeof(unsigned int);
		// AC5 = address;
		address += sizeof(unsigned int);
		// AC6 = address;
		address += sizeof(unsigned int);
		// VB1 = address;
		address += sizeof(int);
		// VB2 = address;
		address += sizeof(int);
		// MB = address;
		address += sizeof(int);
		// MC = address;
		address += sizeof(int);
		// MD = address;
		address += sizeof(int);
		// c3 = address;
		address += sizeof(double);
		// c4 = address;
		address += sizeof(double);
		// b1 = address;
		address += sizeof(double);
		c5 = address;
		address += sizeof(double);
		c6 = address;
		address += sizeof(double);
		mc = address;
		address += sizeof(double);
		md = address;
		address += sizeof(double);
		x0 = address;
		address += sizeof(double);
		x1 = address;
		address += sizeof(double);
		x2 = address;
		address += sizeof(double);
		y0 = address;
		address += sizeof(double);
		y1 = address;
		address += sizeof(double);
		y2 = address;
		address += sizeof(double);
		p0 = address;
		address += sizeof(double);
		p1 = address;
		address += sizeof(double);
		p2 = address;
		address += sizeof(double);

		/*
		// Serial.print("c3: "); Serial.println(eeprom.readDouble(c3));
		// Serial.print("c4: "); Serial.println(eeprom.readDouble(c4));
		Serial.print("c5: "); Serial.println(eeprom.readDouble(c5));
		Serial.print("c6: "); Serial.println(eeprom.readDouble(c6));
		// Serial.print("b1: "); Serial.println(eeprom.readDouble(b1));
		Serial.print("mc: "); Serial.println(eeprom.readDouble(mc));
		Serial.print("md: "); Serial.println(eeprom.readDouble(md));
		Serial.print("x0: "); Serial.println(eeprom.readDouble(x0));
		Serial.print("x1: "); Serial.println(eeprom.readDouble(x1));
		Serial.print("x2: "); Serial.println(eeprom.readDouble(x2));
		Serial.print("y0: "); Serial.println(eeprom.readDouble(y0));
		Serial.print("y1: "); Serial.println(eeprom.readDouble(y1));
		Serial.print("y2: "); Serial.println(eeprom.readDouble(y2));
		Serial.print("p0: "); Serial.println(eeprom.readDouble(p0));
		Serial.print("p1: "); Serial.println(eeprom.readDouble(p1));
		Serial.print("p2: "); Serial.println(eeprom.readDouble(p2));
		Serial.print("control: "); Serial.println(eeprom.readInt(EEPROM_CHECK));
		*/

		return(1);
	} else {
		#ifdef DEBUG
		Serial.println("no calibration data, please run eeprom");
		#endif
		return(0);
	}
	#endif

	#ifdef WRITE_EEPROM
	// Retrieve calibration data from device:
	if (readInt(0xAA,AC1v) &&
		readInt(0xAC,AC2v) &&
		readInt(0xAE,AC3v) &&
		readUInt(0xB0,AC4v) &&
		readUInt(0xB2,AC5v) &&
		readUInt(0xB4,AC6v) &&
		readInt(0xB6,VB1v) &&
		readInt(0xB8,VB2v) &&
		readInt(0xBA,MBv) &&
		readInt(0xBC,MCv) &&
		readInt(0xBE,MDv))
	{
		Serial.println(F("all reads good"));
		uint8_t address = 0;
		AC1 = address;
		eeprom.writeInt(AC1, AC1v);
		address += sizeof(int);
		AC2 = address;
		eeprom.writeInt(AC2, AC2v);
		address += sizeof(int);
		AC3 = address;
		eeprom.writeInt(AC3, AC3v);
		address += sizeof(int);
		AC4 = address;
		eeprom.writeUInt(AC4, AC4v);
		address += sizeof(unsigned int);
		AC5 = address;
		eeprom.writeUInt(AC5, AC5v);
		address += sizeof(unsigned int);
		AC6 = address;
		eeprom.writeUInt(AC6, AC6v);
		address += sizeof(unsigned int);
		VB1 = address;
		eeprom.writeInt(VB1, VB1v);
		address += sizeof(int);
		VB2 = address;
		eeprom.writeInt(VB2, VB2v);
		address += sizeof(int);
		MB = address;
		eeprom.writeInt(MB, MBv);
		address += sizeof(int);
		MC = address;
		eeprom.writeInt(MC, MCv);
		address += sizeof(int);
		MD = address;
		eeprom.writeInt(MD, MDv);
		address += sizeof(int);
		// All reads completed successfully!

		// If you need to check your math using known numbers,
		// you can uncomment one of these examples.
		// (The correct results are commented in the below functions.)

		// Example from Bosch datasheet
		// AC1 = 408; AC2 = -72; AC3 = -14383; AC4 = 32741; AC5 = 32757; AC6 = 23153;
		// B1 = 6190; B2 = 4; MB = -32768; MC = -8711; MD = 2868;

		// Example from http://wmrx00.sourceforge.net/Arduino/BMP180-Calcs.pdf
		// AC1 = 7911; AC2 = -934; AC3 = -14306; AC4 = 31567; AC5 = 25671; AC6 = 18974;
		// VB1 = 5498; VB2 = 46; MB = -32768; MC = -11075; MD = 2432;


		Serial.print("AC1: "); Serial.println(AC1v);
		Serial.print("AC2: "); Serial.println(AC2v);
		Serial.print("AC3: "); Serial.println(AC3v);
		Serial.print("AC4: "); Serial.println(AC4v);
		Serial.print("AC5: "); Serial.println(AC5v);
		Serial.print("AC6: "); Serial.println(AC6v);
		Serial.print("VB1: "); Serial.println(VB1v);
		Serial.print("VB2: "); Serial.println(VB2v);
		Serial.print("MB: "); Serial.println(MBv);
		Serial.print("MC: "); Serial.println(MCv);
		Serial.print("MD: "); Serial.println(MDv);


		// Compute floating-point polynominals:

		uint8_t c3, c4, b1;
		double value = 0;
		c3 = address; value = 160.0 * pow(2,-15) * eeprom.readInt(AC3);
		eeprom.writeDouble(c3, value);
		address += sizeof(value);
		c4 = address; value = pow(10,-3) * pow(2,-15) * eeprom.readUInt(AC4);
		eeprom.writeDouble(c4, value);
		address += sizeof(value);
		b1 = address; value = pow(160,2) * pow(2,-30) * eeprom.readInt(VB1);
		eeprom.writeDouble(b1, value);
		address += sizeof(value);
		c5 = address; value = (pow(2,-15) / 160) * eeprom.readUInt(AC5);
		eeprom.writeDouble(c5, value);
		address += sizeof(value);
		c6 = address; value = eeprom.readInt(AC6);
		eeprom.writeDouble(c6, value);
		address += sizeof(value);
		mc = address; value = (pow(2,11) / pow(160,2)) * eeprom.readInt(MC);
		eeprom.writeDouble(mc, value);
		address += sizeof(value);
		md = address; value = eeprom.readInt(MD) / 160.0;
		eeprom.writeDouble(md, value);
		address += sizeof(value);
		x0 = address; value = eeprom.readInt(AC1);
		eeprom.writeDouble(x0, value);
		address += sizeof(value);
		x1 = address; value = 160.0 * pow(2,-13) * eeprom.readInt(AC2);
		eeprom.writeDouble(x1, value);
		address += sizeof(value);
		x2 = address; value = pow(160,2) * pow(2,-25) * eeprom.readInt(VB2);
		eeprom.writeDouble(x2, value);
		address += sizeof(value);
		y0 = address; value = eeprom.readDouble(c4) * pow(2,15);
		eeprom.writeDouble(y0, value);
		address += sizeof(value);
		y1 = address; value = eeprom.readDouble(c4) * eeprom.readDouble(c3);
		eeprom.writeDouble(y1, value);
		address += sizeof(value);
		y2 = address; value = eeprom.readDouble(c4) * eeprom.readDouble(b1);
		eeprom.writeDouble(y2, value);
		address += sizeof(value);
		p0 = address; value = (3791.0 - 8.0) / 1600.0;
		eeprom.writeDouble(p0, value);
		address += sizeof(value);
		p1 = address; value = 1.0 - 7357.0 * pow(2,-20);
		eeprom.writeDouble(p1, value);
		address += sizeof(value);
		p2 = address; value = 3038.0 * 100.0 * pow(2,-36);
		eeprom.writeDouble(p2, value);
		address += sizeof(value);
		// control address
		eeprom.writeInt(address, 42);
		// c3 = 160.0 * pow(2,-15) * AC3;
		// c4 = pow(10,-3) * pow(2,-15) * AC4;
		// b1 = pow(160,2) * pow(2,-30) * VB1;
		// c5 = (pow(2,-15) / 160) * AC5;
		// c6 = AC6;
		// mc = (pow(2,11) / pow(160,2)) * MC;
		// md = MD / 160.0;
		// x0 = AC1;
		// x1 = 160.0 * pow(2,-13) * AC2;
		// x2 = pow(160,2) * pow(2,-25) * VB2;
		// y0 = c4 * pow(2,15);
		// y1 = c4 * c3;
		// y2 = c4 * b1;
		// p0 = (3791.0 - 8.0) / 1600.0;
		// p1 = 1.0 - 7357.0 * pow(2,-20);
		// p2 = 3038.0 * 100.0 * pow(2,-36);

		Serial.println();
		Serial.print("c3: "); Serial.println(eeprom.readDouble(c3));
		Serial.print("c4: "); Serial.println(eeprom.readDouble(c4));
		Serial.print("c5: "); Serial.println(eeprom.readDouble(c5));
		Serial.print("c6: "); Serial.println(eeprom.readDouble(c6));
		Serial.print("b1: "); Serial.println(eeprom.readDouble(b1));
		Serial.print("mc: "); Serial.println(eeprom.readDouble(mc));
		Serial.print("md: "); Serial.println(eeprom.readDouble(md));
		Serial.print("x0: "); Serial.println(eeprom.readDouble(x0));
		Serial.print("x1: "); Serial.println(eeprom.readDouble(x1));
		Serial.print("x2: "); Serial.println(eeprom.readDouble(x2));
		Serial.print("y0: "); Serial.println(eeprom.readDouble(y0));
		Serial.print("y1: "); Serial.println(eeprom.readDouble(y1));
		Serial.print("y2: "); Serial.println(eeprom.readDouble(y2));
		Serial.print("p0: "); Serial.println(eeprom.readDouble(p0));
		Serial.print("p1: "); Serial.println(eeprom.readDouble(p1));
		Serial.print("p2: "); Serial.println(eeprom.readDouble(p2));
		Serial.print("control address: "); Serial.println(address);

		// Success!
		return(1);
	}
	else
	{
		// Error reading calibration data; bad component or connection?
		return(0);
	}
	#endif
}


char SFE_BMP180::readInt(char address, int16_t &value)
// Read a signed integer (two bytes) from device
// address: register to start reading (plus subsequent register)
// value: external variable to store data (function modifies value)
{
	unsigned char data[2];

	data[0] = address;
	if (readBytes(data,2))
	{
		value = (int16_t)((data[0]<<8)|data[1]);
		//if (*value & 0x8000) *value |= 0xFFFF0000; // sign extend if negative
		return(1);
	}
	value = 0;
	return(0);
}


char SFE_BMP180::readUInt(char address, uint16_t &value)
// Read an unsigned integer (two bytes) from device
// address: register to start reading (plus subsequent register)
// value: external variable to store data (function modifies value)
{
	unsigned char data[2];

	data[0] = address;
	if (readBytes(data,2))
	{
		value = (((uint16_t)data[0]<<8)|(uint16_t)data[1]);
		return(1);
	}
	value = 0;
	return(0);
}


char SFE_BMP180::readBytes(unsigned char *values, char length)
// Read an array of bytes from device
// values: external array to hold data. Put starting register in values[0].
// length: number of bytes to read
{
	uint8_t x;

	Wire.beginTransmission(BMP180_ADDR);
	Wire.write(values[0]);
	_error = Wire.endTransmission();
	if (_error == 0)
	{
		Wire.requestFrom(BMP180_ADDR,length);
		while(Wire.available() != length) ; // wait until bytes are ready
		for(x=0;x<length;x++)
		{
			values[x] = Wire.read();
		}
		return(1);
	}
	return(0);
}


char SFE_BMP180::writeBytes(unsigned char *values, char length)
// Write an array of bytes to device
// values: external array of data to write. Put starting register in values[0].
// length: number of bytes to write
{
	Wire.beginTransmission(BMP180_ADDR);
	Wire.write(values,length);
	_error = Wire.endTransmission();
	if (_error == 0)
		return(1);
	else
		return(0);
}


char SFE_BMP180::startTemperature(void)
// Begin a temperature reading.
// Will return delay in ms to wait, or 0 if I2C error
{
	unsigned char data[2], result;

	data[0] = BMP180_REG_CONTROL;
	data[1] = BMP180_COMMAND_TEMPERATURE;
	result = writeBytes(data, 2);
	if (result) // good write?
		return(5); // return the delay in ms (rounded up) to wait before retrieving data
	else
		return(0); // or return 0 if there was a problem communicating with the BMP
}


char SFE_BMP180::getTemperature(double &T)
// Retrieve a previously-started temperature reading.
// Requires begin() to be called once prior to retrieve calibration parameters.
// Requires startTemperature() to have been called prior and sufficient time elapsed.
// T: external variable to hold result.
// Returns 1 if successful, 0 if I2C error.
{
	unsigned char data[2];
	char result;
	double tu, a;

	data[0] = BMP180_REG_RESULT;

	result = readBytes(data, 2);
	if (result) // good read, calculate temperature
	{
		tu = (data[0] * 256.0) + data[1];

		//example from Bosch datasheet
		//tu = 27898;

		//example from http://wmrx00.sourceforge.net/Arduino/BMP085-Calcs.pdf
		//tu = 0x69EC;

		a = eeprom.readDouble(c5) * (tu - eeprom.readDouble(c6));
		T = a + (eeprom.readDouble(mc) / (a + eeprom.readDouble(md)));

		/*
		Serial.println();
		Serial.print("tu: "); Serial.println(tu);
		Serial.print("a: "); Serial.println(a);
		Serial.print("T: "); Serial.println(T);
		*/
	}
	return(result);
}


char SFE_BMP180::startPressure(char oversampling)
// Begin a pressure reading.
// Oversampling: 0 to 3, higher numbers are slower, higher-res outputs.
// Will return delay in ms to wait, or 0 if I2C error.
{
	unsigned char data[2], result, delay;

	data[0] = BMP180_REG_CONTROL;

	switch (oversampling)
	{
		case 0:
			data[1] = BMP180_COMMAND_PRESSURE0;
			delay = 5;
		break;
		case 1:
			data[1] = BMP180_COMMAND_PRESSURE1;
			delay = 8;
		break;
		case 2:
			data[1] = BMP180_COMMAND_PRESSURE2;
			delay = 14;
		break;
		case 3:
			data[1] = BMP180_COMMAND_PRESSURE3;
			delay = 26;
		break;
		default:
			data[1] = BMP180_COMMAND_PRESSURE0;
			delay = 5;
		break;
	}
	result = writeBytes(data, 2);
	if (result) // good write?
		return(delay); // return the delay in ms (rounded up) to wait before retrieving data
	else
		return(0); // or return 0 if there was a problem communicating with the BMP
}


char SFE_BMP180::getPressure(double &P, double &T)
// Retrieve a previously started pressure reading, calculate abolute pressure in mbars.
// Requires begin() to be called once prior to retrieve calibration parameters.
// Requires startPressure() to have been called prior and sufficient time elapsed.
// Requires recent temperature reading to accurately calculate pressure.

// P: external variable to hold pressure.
// T: previously-calculated temperature.
// Returns 1 for success, 0 for I2C error.

// Note that calculated pressure value is absolute mbars, to compensate for altitude call sealevel().
{
	unsigned char data[3];
	char result;
	double pu,s,x,y,z;

	data[0] = BMP180_REG_RESULT;

	result = readBytes(data, 3);
	if (result) // good read, calculate pressure
	{
		pu = (data[0] * 256.0) + data[1] + (data[2]/256.0);

		//example from Bosch datasheet
		//pu = 23843;

		//example from http://wmrx00.sourceforge.net/Arduino/BMP085-Calcs.pdf, pu = 0x982FC0;
		//pu = (0x98 * 256.0) + 0x2F + (0xC0/256.0);

		s = T - 25.0;
		x = (eeprom.readDouble(x2) * pow(s,2)) + (eeprom.readDouble(x1) * s) + eeprom.readDouble(x0);
		y = (eeprom.readDouble(y2) * pow(s,2)) + (eeprom.readDouble(y1) * s) + eeprom.readDouble(y0);
		z = (pu - x) / y;
		P = (eeprom.readDouble(p2) * pow(z,2)) + (eeprom.readDouble(p1) * z) + eeprom.readDouble(p0);

		/*
		Serial.println();
		Serial.print("pu: "); Serial.println(pu);
		Serial.print("T: "); Serial.println(*T);
		Serial.print("s: "); Serial.println(s);
		Serial.print("x: "); Serial.println(x);
		Serial.print("y: "); Serial.println(y);
		Serial.print("z: "); Serial.println(z);
		Serial.print("P: "); Serial.println(*P);
		*/
	}
	return(result);
}


double SFE_BMP180::sealevel(double P, double A)
// Given a pressure P (mb) taken at a specific altitude (meters),
// return the equivalent pressure (mb) at sea level.
// This produces pressure readings that can be used for weather measurements.
{
	return(P/pow(1-(A/44330.0),5.255));
}


double SFE_BMP180::altitude(double P, double P0)
// Given a pressure measurement P (mb) and the pressure at a baseline P0 (mb),
// return altitude (meters) above baseline.
{
	return(44330.0*(1-pow(P/P0,1/5.255)));
}


char SFE_BMP180::getError(void)
	// If any library command fails, you can retrieve an extended
	// error code using this command. Errors are from the wire library:
	// 0 = Success
	// 1 = Data too long to fit in transmit buffer
	// 2 = Received NACK on transmit of address
	// 3 = Received NACK on transmit of data
	// 4 = Other error
{
	return(_error);
}
