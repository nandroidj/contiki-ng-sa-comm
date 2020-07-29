#ifndef SENSING_H_
#define SENSING_H_

#include "contiki.h"
#include "sys/etimer.h"
#include "sys/ctimer.h"
#include "random.h"

//#include <ti-lib.h>

#include "contiki.h"
#include "dev/leds.h"
#include "lib/sensors.h"

#include "platform/simplelink/cc13xx-cc26xx/launchpad/board-peripherals.h"
#include "batmon-sensor.h"

#include <stdio.h>
#include <stdint.h>

void init_sensor_readings(void);
void init_battery_sensor(void);

void init_opt_reading(void *not_used);
void init_hdc_reading(void *not_used);
void init_soil_moisture_reading(void *not_used);
void init_temp_ds18b20_reading(void *not_used);

void get_hdc_reading(void);
void get_light_reading(void);
void get_soil_moisture_reading(void);
void get_temp_ds18b20_reading(void);
void get_battery_sensor_reading(void);

#endif /* SENSING_H_ */
