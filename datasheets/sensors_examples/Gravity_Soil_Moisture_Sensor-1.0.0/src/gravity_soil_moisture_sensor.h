#ifndef GRAVITY_SOIL_MOISTURE_SENSOR_H
    #define GRAVITY_SOIL_MOISTURE_SENSOR_H

    #include <stdint.h>

    class GravitySoilMoistureSensor
    {
        uint8_t _analog_pin;

        public:
            bool Setup(const uint8_t analog_pin);
            uint16_t Read(uint16_t samples = 128, uint16_t delay_ms = 1);
    };
#endif
