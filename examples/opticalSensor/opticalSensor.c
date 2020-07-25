/*
 * opticalSensor.c
 *
 *  Created on: Jul 13, 2020
 *      Author: nandroid
 */

#include "opticalSensor.h"

/*---------------------------------------------------------------------------*/
#define CC26XX_DEMO_LOOP_INTERVAL       (CLOCK_SECOND * 20)
#define CC26XX_DEMO_LEDS_PERIODIC       LEDS_YELLOW
#define CC26XX_DEMO_LEDS_BUTTON         LEDS_RED
#define CC26XX_DEMO_LEDS_REBOOT         LEDS_ALL
/*---------------------------------------------------------------------------*/
static struct etimer et;
/*---------------------------------------------------------------------------*/
#define SENSOR_READING_PERIOD (CLOCK_SECOND * 2)

static struct ctimer opt_timer, hdc_timer;

/*---------------------------------------------------------------------------*/
PROCESS(opticalSensor, "cc1352r1 demo process");
AUTOSTART_PROCESSES(&opticalSensor);
/*---------------------------------------------------------------------------*/
static void init_opt_reading(void *not_used);
static void init_hdc_reading(void *not_used);
/*---------------------------------------------------------------------------*/
static void
init_opt_reading(void *not_used)
{
  SENSORS_ACTIVATE(opt_3001_sensor);
}
/*---------------------------------------------------------------------------*/
static void
init_hdc_reading(void *not_used)
{
  SENSORS_ACTIVATE(hdc_1000_sensor);
}
/*---------------------------------------------------------------------------*/
static void
get_hdc_reading()
{
  int value;
  clock_time_t next = SENSOR_READING_PERIOD;

  value = hdc_1000_sensor.value(HDC_1000_SENSOR_TYPE_TEMP);
  if(value != -1) {
    printf("HDC: Temp=%d.%02d C\n", value / 100, value % 100);
  } else {
    printf("HDC: Temp Read Error\n");
  }

  value = hdc_1000_sensor.value(HDC_1000_SENSOR_TYPE_HUMID);
  if(value != -1) {
    printf("HDC: Humidity=%d.%02d %%RH\n", value / 100, value % 100);
  } else {
    printf("HDC: Humidity Read Error\n");
  }

  ctimer_set(&hdc_timer, next, init_hdc_reading, NULL);
}
/*---------------------------------------------------------------------------*/
static void
get_light_reading()
{
  int value;
  clock_time_t next = SENSOR_READING_PERIOD;

  value = opt_3001_sensor.value(0);
  if(value != -1) {
    printf("OPT: Light=%d.%02d lux\n", value / 100, value % 100);
  } else {
    printf("OPT: Light Read Error\n");
  }

  /* The OPT will turn itself off, so we don't need to call its DEACTIVATE */
  ctimer_set(&opt_timer, next, init_opt_reading, NULL);
}
/*---------------------------------------------------------------------------*/
static void
get_sync_sensor_readings(void)
{
  int value;

  printf("-----------------------------------------\n");

  value = batmon_sensor.value(BATMON_SENSOR_TYPE_TEMP);
  printf("Bat: Temp=%d C\n", value);

  value = batmon_sensor.value(BATMON_SENSOR_TYPE_VOLT);
  printf("Bat: Volt=%d mV\n", (value * 125) >> 5);

  return;
}

/*---------------------------------------------------------------------------*/
static void
init_sensors(void)
{
  SENSORS_ACTIVATE(batmon_sensor);
}
/*---------------------------------------------------------------------------*/
static void
init_sensor_readings(void)
{
  SENSORS_ACTIVATE(hdc_1000_sensor);
  SENSORS_ACTIVATE(opt_3001_sensor);

  printf("sensors activated! \n");
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(opticalSensor, ev, data)
{

  PROCESS_BEGIN();

  printf("CC1352R1 Demo\n");

  init_sensors(); // initialization of battery monitor sensor.

  etimer_set(&et, CC26XX_DEMO_LOOP_INTERVAL);

  printf("first batmon reading \n");
  get_sync_sensor_readings();

  printf("initialize readings \n");
  init_sensor_readings();

  while(1) {

    PROCESS_YIELD();

//    if(ev == PROCESS_EVENT_TIMER) {
      //if(data == &et) {
        leds_toggle(CC26XX_DEMO_LEDS_PERIODIC);

        printf("get batmon reading \n");
        get_sync_sensor_readings();

        etimer_set(&et, CC26XX_DEMO_LOOP_INTERVAL);
  }
//    } else if(ev == sensors_event) {
//         if(data == &opt_3001_sensor) {
          printf("get light reading \n");
          get_light_reading();
//         } else if(data == &hdc_1000_sensor) {
          printf("get hdc reading \n");
          get_hdc_reading();
//         }
//    }
//  }

  PROCESS_END();
}
