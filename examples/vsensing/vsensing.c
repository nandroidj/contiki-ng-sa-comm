/*
 * onBoard_sensors.c
 *
 *  Created on: Jul 19, 2020
 *      Author: nandroid
 */

#include "contiki.h"
#include "vsensing.h"

PROCESS(sensor_process, "Sensor process");
AUTOSTART_PROCESSES(&sensor_process);

#define RANDOM_NUMBER_DEN 10000
#define MIN_temp_value 0
#define MAX_temp_value 3

#define MINIMUM 0
#define MAXIMUM 10

PROCESS_THREAD(sensor_process, ev, data)
{
    static struct etimer timer;

    unsigned short temp_value;
    unsigned short hum_value;

    PROCESS_BEGIN();

    /* Setup a periodic timer that expires after 10 seconds. */
    etimer_set(&timer, CLOCK_SECOND * 2);

    while(1) {
//      printf(" \n Hello, world \n");

      temp_value = random_rand();
      hum_value = random_rand();

      if(temp_value <= 100) {
          temp_value /= 10;
          hum_value /= 10;

      } else if(temp_value <= 1000) {
          temp_value /= 100;
          hum_value /= 100;

      } else if(temp_value <= RANDOM_RAND_MAX) {
          temp_value /= 1000;
          hum_value /= 1000;
      }

      printf("temperature = 20.%hu", temp_value);
      printf("humidity = 80.%hu  \n", hum_value);

      /* Wait for the periodic timer to expire and then restart the timer. */
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
      etimer_reset(&timer);
    }

    PROCESS_END();
}
