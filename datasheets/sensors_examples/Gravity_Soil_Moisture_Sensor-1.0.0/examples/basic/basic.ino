#include <Arduino.h>
#include "gravity_soil_moisture_sensor.h"

GravitySoilMoistureSensor gravity_sensor;

void setup() {
    // update this
    uint8_t pin = 0;

    if (!gravity_sensor.Setup(pin)) {
        Serial.println("Gravity Soil Moisture Sensor was not detected. Check wiring!");
        while(1);
    } else {
        Serial.println("Gravity Soil Moisture Sensor init done.");
    }
}

void loop() {
    uint16_t value = gravity_sensor.Read();

    Serial.printf("Moisture: %d\n", value);

    delay(1000);
}
