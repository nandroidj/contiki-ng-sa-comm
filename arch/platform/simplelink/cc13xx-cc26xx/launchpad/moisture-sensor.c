/*---------------------------------------------------------------------------*/
/*  Sensor in dry soil 0 ~ 300
    Sensor in humid soil 300 ~ 700
    Sensor in water 700 ~ 950
*/
/*---------------------------------------------------------------------------*/
#include "contiki.h"
#include "lib/sensors.h"
#include "sys/ctimer.h"
/*---------------------------------------------------------------------------*/
#include "board-conf.h"
#include "moisture-sensor.h"
/*---------------------------------------------------------------------------*/
#include <Board.h>

#include <ti/drivers/ADC.h>
/*---------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
/*---------------------------------------------------------------------------*/
#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
/*---------------------------------------------------------------------------*/
#define PIN_DIO28 Board_DIO28_ANALOG
#define MOISTURE_THRESHOLD 700
/*---------------------------------------------------------------------------*/
typedef struct {
  volatile SOIL_MOISTURE_STATUS status;
} SOIL_MOISTURE_Object;

static SOIL_MOISTURE_Object soil_moisture;
/*---------------------------------------------------------------------------*/
/* Wait SENSOR_STARTUP_DELAY for the sensor to be ready - 125ms */
#define SENSOR_STARTUP_DELAY (CLOCK_SECOND >> 3)

/*---------------------------------------------------------------------------*/
/* ADC sample count */
#define ADC_SAMPLE_COUNT  (10)

#define THREADSTACKSIZE   (768)

/* ADC conversion result variables */
uint16_t adcValue0;
uint32_t adcValue0MicroVolt;
uint16_t adcValue1[ADC_SAMPLE_COUNT];
uint32_t adcValue1MicroVolt[ADC_SAMPLE_COUNT];
/*---------------------------------------------------------------------------*/
static ADC_Handle   adc;
/*---------------------------------------------------------------------------*/
/**
 * \brief         Configuration function for the OPT3001 sensor.
 * \param type    Activate, enable or disable the sensor. See below.
 * \param enable  Enable or disable sensor.
 *
 *                When type == SENSORS_HW_INIT we turn on the hardware.
 *                When type == SENSORS_ACTIVE and enable==1 we enable the sensor.
 *                When type == SENSORS_ACTIVE and enable==0 we disable the sensor.
 */
static int configure(int type, int enable) {

    ADC_Params   params;

    /* Call driver init functions */
    ADC_init();

    ADC_Params_init(&params);
    adc = ADC_open(PIN_DIO28, &params);

    if (adc == NULL) {
        soil_moisture.status = SOIL_MOISTURE_STATUS_ADC_ERROR;
        while (1);
    }

    int rv = 0;
    switch(type) {

        case SENSORS_HW_INIT:
            if(1) {
                ADC_init();
                soil_moisture.status = SOIL_MOISTURE_STATUS_STANDBY;
            } else {
              soil_moisture.status = SOIL_MOISTURE_STATUS_DISABLED;
              rv = SOIL_MOISTURE_READING_ERROR;
            }
            break;

        case SENSORS_ACTIVE:
            if(enable) {
              rv = SOIL_MOISTURE_STATUS_ACTIVE;
              soil_moisture.status = SOIL_MOISTURE_STATUS_BOOTING;
            }
            break;

        default:
            break;
    }

    return rv;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief       Returns the status of the sensor.
 * \param type  Ignored.
 * \return      The state of the sensor SENSOR_STATE_xyz.
 */
static int
status(int type)
{
  /* Unused args */
  (void)type;

  return soil_moisture.status;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief       Returns a reading from the sensor.
 * \param type  Ignored.
 * \return      Moisture in percent.
 */
static int
value(int type)
{
    int_fast16_t res;

    (void)type;

    if(soil_moisture.status != SOIL_MOISTURE_STATUS_DATA_READY) {
        return SOIL_MOISTURE_READING_ERROR;
    }

    /* Blocking mode conversion */
    res = ADC_convert(adc, &adcValue0);

    if (res == ADC_STATUS_SUCCESS) {

       adcValue0MicroVolt = ADC_convertRawToMicroVolts(adc, adcValue0);

       if(adcValue0MicroVolt <= MOISTURE_THRESHOLD) {
           PRINTF("HUM: %d \n", (int)(adcValue0MicroVolt));
       }

    } else {
        soil_moisture.status = SOIL_MOISTURE_STATUS_ADC_ERROR;
        return SOIL_MOISTURE_READING_ERROR;
    }

    ADC_close(adc);

    return (int)(adcValue0MicroVolt);
}
/*-------------------------------------------------------------------------------*/
SENSORS_SENSOR(soil_moisture_sensor, "MOISTURE-SENSOR", value, configure, status);
/*-------------------------------------------------------------------------------*/
/** @} */
