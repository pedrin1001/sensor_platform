#include "Barometer.h"
#include <MemoryFree.h>

Barometer::Barometer() {
}

int Barometer::begin() {
    // if it fails
    if(!(_pressure.begin())) {
        return 0;
    }
    _baseline = _getPressure();

    #ifdef DEBUG
    Serial.print("baseline pressure: ");
    Serial.print(_baseline);
    Serial.println(" mb");
    #endif
    return 1;
}

int Barometer::getAltitude(double &alt) {
    // Get a new pressure reading:

    double P = _getPressure();
    if (!P) {
        return 0;
    }

    // Show the relative altitude difference between
    // the new reading and the baseline reading:

    alt = _pressure.altitude(P, _baseline);
    if (!alt) {
        return 0;
    }
    return 1;
}

double Barometer::_getPressure() {
    char status;
    double T, P;

    // You must first get a temperature measurement to perform a pressure reading.

    // Start a temperature measurement:
    // If request is successful, the number of ms to wait is returned.
    // If request is unsuccessful, 0 is returned.

    status = _pressure.startTemperature();
    if (status != 0) {
        // Wait for the measurement to complete:

        delay(status);//Serial.println(status);

        // Retrieve the completed temperature measurement:
        // Note that the measurement is stored in the variable T.
        // Use '&T' to provide the address of T to the function.
        // Function returns 1 if successful, 0 if failure.

        status = _pressure.getTemperature(T);
        if (status != 0) {
            // Start a pressure measurement:
            // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
            // If request is successful, the number of ms to wait is returned.
            // If request is unsuccessful, 0 is returned.

            status = _pressure.startPressure(3);
            if (status != 0) {
                // Wait for the measurement to complete:
                delay(status);//Serial.println(status);

                // Retrieve the completed pressure measurement:
                // Note that the measurement is stored in the variable P.
                // Use '&P' to provide the address of P.
                // Note also that the function requires the previous temperature measurement (T).
                // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
                // Function returns 1 if successful, 0 if failure.

                status = _pressure.getPressure(P, T);
                if (status != 0) {
                    return(P);
                } else {
                    Serial.println("error retrieving pressure measurement\n");
                    return 0;
                }
            } else {
                Serial.println("error starting pressure measurement\n");
                return 0;
            }
        } else {
            Serial.println("error retrieving temperature measurement\n");
            return 0;
        };
    } else {
        Serial.println("error starting temperature measurement\n");
        return 0;
    }
}
