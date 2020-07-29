/*---------------------------------------------------------------------------*/
#ifndef SOIL_MOISTURE_SENSOR_H_
#define SOIL_MOISTURE_SENSOR_H_
/*---------------------------------------------------------------------------*/
#include "contiki.h"
/*---------------------------------------------------------------------------*/
#include "board-conf.h"
/*---------------------------------------------------------------------------*/
#define SOIL_MOISTURE_READING_ERROR  -1
/*---------------------------------------------------------------------------*/
typedef enum {
  SOIL_MOISTURE_STATUS_DISABLED,
  SOIL_MOISTURE_STATUS_STANDBY,
  SOIL_MOISTURE_STATUS_BOOTING,
  SOIL_MOISTURE_STATUS_ACTIVE,
  SOIL_MOISTURE_STATUS_DATA_READY,
  SOIL_MOISTURE_STATUS_ADC_ERROR,
} SOIL_MOISTURE_STATUS;
/*---------------------------------------------------------------------------*/
extern const struct sensors_sensor soil_moisture_sensor;
/*---------------------------------------------------------------------------*/
#endif /* SOIL_MOISTURE_SENSOR_H_ */
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 */
