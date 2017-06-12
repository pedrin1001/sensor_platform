#ifndef Utils_h
#define Utils_h

/**
 * reads voltage at given pin, used for the MQ-* gas sensors
 */
int readMQ(int pin);

/**
 * maps 0..255 values to 0..1024 (arduino analog voltage values)
 */
uint16_t rgbToVoltage(const uint8_t &value);

/**
 * set LEDs to given rgb values
 */
void setLEDColor(const uint8_t &r, const uint8_t &g, const uint8_t &b);

/**
 * set LEDs to red and stop the program with an infinite loop
 */
void error(char const* msg);

#endif
