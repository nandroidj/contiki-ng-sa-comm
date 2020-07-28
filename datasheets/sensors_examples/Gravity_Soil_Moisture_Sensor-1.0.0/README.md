# Gravity Soil Moisture Sensor
Arduino Library for DfRobot's [Gravity Analog Capacitive Soil Moisture Sensor](https://wiki.dfrobot.com/Capacitive_Soil_Moisture_Sensor_SKU_SEN0193).

# Features
* the returned value represents moisture (compared to the raw value that represents dryness)
* multi-sampling
* slight delay between samples
* protection against invalid values (valid values are between 1 and 3500)
