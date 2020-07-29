#include "contiki.h"
#include "sensing.h"

/*---------------------------------------------------------------------------*/
static struct etimer et;
/*---------------------------------------------------------------------------*/
PROCESS(sensor_process, "sensor_process");
/*---------------------------------------------------------------------------*/
#define SENSOR_READING_ERROR -1
#define PROCESS_INTERVAL       (CLOCK_SECOND * 20)
/*---------------------------------------------------------------------------*/
/*
 * Update sensor readings in a staggered fashion every SENSOR_READING_PERIOD
 * ticks + a random interval between 0 and SENSOR_READING_RANDOM ticks
 */
#define SENSOR_READING_PERIOD (CLOCK_SECOND * 20)
#define SENSOR_READING_RANDOM (CLOCK_SECOND << 4)

static struct ctimer opt_timer, hdc_timer, moisture_timer, temp_timer;
/*---------------------------------------------------------------------------*/
static void init_opt_reading(void *not_used);
static void init_hdc_reading(void *not_used);
static void init_soil_moisture_reading(void *not_used);
static void init_temp_ds18b20_reading(void *not_used);
/*---------------------------------------------------------------------------*/
static void
get_hdc_reading()
{
  int value;
  clock_time_t next = SENSOR_READING_PERIOD +
    (random_rand() % SENSOR_READING_RANDOM);

  value = hdc_1000_sensor.value(HDC_1000_SENSOR_TYPE_TEMP);
  if(value != SENSOR_READING_ERROR) {
    printf("HDC: Temp=%d.%02d C\n", value / 100, value % 100);
  } else {
    printf("HDC: Temp Read Error\n");
  }

  value = hdc_1000_sensor.value(HDC_1000_SENSOR_TYPE_HUMID);
  if(value != SENSOR_READING_ERROR) {
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
  clock_time_t next = SENSOR_READING_PERIOD +
    (random_rand() % SENSOR_READING_RANDOM);

  value = opt_3001_sensor.value(0);
  if(value != SENSOR_READING_ERROR) {
    printf("OPT: Light=%d.%02d lux\n", value / 100, value % 100);
  } else {
    printf("OPT: Light Read Error\n");
  }

  /* The OPT will turn itself off, so we don't need to call its DEACTIVATE */
  ctimer_set(&opt_timer, next, init_opt_reading, NULL);
}
/*---------------------------------------------------------------------------*/
static void
get_soil_moisture_reading()
{
  int value;
  clock_time_t next = SENSOR_READING_PERIOD +
    (random_rand() % SENSOR_READING_RANDOM);

  value = soil_moisture_sensor.value(0);
  if(value != SENSOR_READING_ERROR) {
    printf("MOISTURE: Hum=%d \n", value % 10);
  } else {
    printf("MOISTURE: Huimidity Read Error\n");
  }

  /* The OPT will turn itself off, so we don't need to call its DEACTIVATE */
  ctimer_set(&moisture_timer, next, init_soil_moisture_reading, NULL);
}
/*---------------------------------------------------------------------------*/
static void
get_temp_ds18b20_reading()
{
  int value;
  clock_time_t next = SENSOR_READING_PERIOD +
    (random_rand() % SENSOR_READING_RANDOM);

  value = soil_moisture_sensor.value(0);
  if(value != SENSOR_READING_ERROR) {
    printf("TEMP: Temp=%d.%d \n", value, value % 100);
  } else {
    printf("TEMP: Temperature Read Error\n");
  }

  /* The OPT will turn itself off, so we don't need to call its DEACTIVATE */
  ctimer_set(&temp_timer, next, init_temp_ds18b20_reading, NULL);
}
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
init_soil_moisture_reading(void *not_used)
{
  SENSORS_ACTIVATE(soil_moisture_sensor);
}
/*---------------------------------------------------------------------------*/
static void
init_temp_ds18b20_reading(void *not_used)
{
  SENSORS_ACTIVATE(temp_ds18b20_sensor);
}
/*---------------------------------------------------------------------------*/
static void
get_battery_sensor_reading(void)
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
init_battery_sensor(void)
{
  SENSORS_ACTIVATE(batmon_sensor);
}
/*---------------------------------------------------------------------------*/
static void
init_sensor_readings(void)
{
    // On-board Sensors
    SENSORS_ACTIVATE(opt_3001_sensor);
    SENSORS_ACTIVATE(hdc_1000_sensor);

    // External Sensors
    SENSORS_ACTIVATE(soil_moisture_sensor);
    SENSORS_ACTIVATE(temp_ds18b20_sensor);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sensor_process, ev, data)
{

  PROCESS_BEGIN();

  printf("Sensor Process \n");

  init_battery_sensor();

  etimer_set(&et, PROCESS_INTERVAL);
  get_battery_sensor_reading();
  init_sensor_readings();

  while(1) {

    PROCESS_YIELD();

    if(ev == PROCESS_EVENT_TIMER) {
      if(data == &et) {

        get_battery_sensor_reading();

        etimer_set(&et, PROCESS_INTERVAL);
      }
    } else if(ev == sensors_event) {
      if(data == &opt_3001_sensor) {
        get_light_reading();
      } else if(data == &hdc_1000_sensor) {
        get_hdc_reading();
      } else if(data == &soil_moisture_sensor) {
        get_soil_moisture_reading();
      } else if(data == &temp_ds18b20_sensor) {
        get_temp_ds18b20_reading();
      }
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 * @}
 */
