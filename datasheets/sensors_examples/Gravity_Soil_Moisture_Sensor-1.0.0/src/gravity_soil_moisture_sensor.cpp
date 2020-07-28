#include <Arduino.h>
#include "gravity_soil_moisture_sensor.h"

/**
 * @brief Function to initialize the Gravity Soil Moisture Sensor
 * @param analog_pin : GPIO analog pin
 * @return true if a valid value is available. false otherwise
 */
bool GravitySoilMoistureSensor::Setup(const uint8_t analog_pin)
{
    _analog_pin = analog_pin;

    return Read() > 0;
}

/**
 * @brief Function to read the soil moisture value
 * @param samples  : Number of samples to take. Defaults to 128
 * @param delay_ms : Delay in milliseconds between each sample reading. Defaults to 1
 * @return The moisture value. A valid reading is between 1 and 3500
 */
uint16_t GravitySoilMoistureSensor::Read(uint16_t samples, uint16_t delay_ms)
{
    uint64_t accumulator = 0;

    for (uint16_t i = 0; i < samples; i++) {
        delay(delay_ms);
        accumulator += analogRead(_analog_pin);
    }

    // the sensor gives the dryness measure
    // substract it from the max value so that we get the moisture measure
    uint16_t value = 4095 - accumulator / samples;

    // use 3500 instead of 4095 because there can be noise in the wire
    if (value <= 3500) {
        return value;
    } else {
        return 0;
    }
}
