/*---------------------------------------------------------------------------*/
/*  OneWire library,
 * https://www.maximintegrated.com/en/design/technical-documents/app-notes/1/126.html
 * https://www.arduinolibraries.info/libraries/one-wire
*/
/*---------------------------------------------------------------------------*/
#include "contiki.h"
#include "lib/sensors.h"
#include "sys/ctimer.h"
/*---------------------------------------------------------------------------*/
#include "board-conf.h"
#include "temp-ds18b20-sensor.h"
/*---------------------------------------------------------------------------*/
#include <Board.h>

#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
/*---------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "contiki.h"
#include "sys/timer.h"
/*---------------------------------------------------------------------------*/
#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
/*---------------------------------------------------------------------------*/
#define PIN_DIO29 Board_DIO29_ANALOG
#define GPIO29_HIGH 1
#define GPIO29_LOW 0
/*---------------------------------------------------------------------------*/
typedef struct {
  volatile TEMP_DS18B20_STATUS status;
} TEMP_DS18B20_SensorData;

static TEMP_DS18B20_SensorData temp_ds18b20;
/*---------------------------------------------------------------------------*/
#include "OneWire.h"
/*---------------------------------------------------------------------------*/
/* Reading bus 1-wire*/
uint8_t Read_Byte(void);
uint8_t Read_bit(void);

/* Writing 1-wire*/
void vWrite_Byte(int val);
void vWrite_Bit(int bit_val);

static void vInputMode(void);
static void vOutputMode(void);
bool Reset_Pulse(void);

uint8_t byte;
const uint8_t read_byte[8];
/*---------------------------------------------------------------------------*/
PIN_Config GPIO_OutLine[] =
{
    Board_DIO29_ANALOG  | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_OPENDRAIN, /* DQ line initially off */
    PIN_TERMINATE                                                                      /* Terminate list */
};

PIN_Config GPIO_InLine[] =
{
    Board_DIO29_ANALOG  | PIN_INPUT_EN | PIN_NOPULL | PIN_HYSTERESIS,
    PIN_TERMINATE                                                                      /* Terminate list */
};

static PIN_State   pinState;
static PIN_Handle  hDqPin;
/*---------------------------------------------------------------------------*/
void delay_one_wire(uint16_t u16timedelay){

    static struct etimer timer;

    /* Setup a periodic timer that expires after 10 seconds. */
    etimer_set(&timer, CLOCK_SECOND * u16timedelay);

    etimer_reset(&timer);

}
/****************************************************************************
Name: vInit_OneWire
Function: Init 1-Wire bus.
Return:
****************************************************************************/
void vInit_OneWire(void){
    hDqPin = PIN_open(&pinState, GPIO_OutLine);
    PIN_setPortOutputValue(hDqPin, GPIO29_HIGH);
}

/****************************************************************************
Name: vSetDs18b20
Function: Setting DS18B20 writting the scratchpad
Return:
****************************************************************************/
void vSetDs18b20(uint8_t u8SetResolution){

    if(Reset_Pulse())
    {
        byte = 0xCC;                                  // Ejecutar el comando SKIP_ROM
        vWrite_Byte(byte);

        byte = 0x4E;                                  //Ejecutar el comando WRITE_SCRATCHPAD
        vWrite_Byte(byte);

        byte = 0xAA;                                  //Envía TH
        vWrite_Byte(byte);

        byte = 0xAA;                                  //Envía TL
        vWrite_Byte(byte);

        byte = u8SetResolution;                                  //Envía Configuration bit
        vWrite_Byte(byte);
    }
}

/****************************************************************************
Name: vConvertTemp
Function: Starting the convertion into DS18B20.
Return:
****************************************************************************/
void vConvertTemp(void){

    if(Reset_Pulse())
    {
        byte=0xCC;                                  // Issue SKIP_ROM 0xcc
        vWrite_Byte(byte);

        byte=0x44;                                  //Issue CONVERT_T
        vWrite_Byte(byte);

        //Comment if the delay is in the application.
//        sleep(1);

        vOutputMode();
    }

    PIN_setPortOutputValue(hDqPin, GPIO29_HIGH);
}

/****************************************************************************
Name: vReadTemp
Function: Read temperature from scratchpad in DS18B20.
Return: Temperature in raw data of 16bit.

    LS BYTE 2^3 2^2 2^1 2^0 2^-1 2^-2 2^-3 2^-4
    MS BYTE S   S   S   S   S    2^6  2^5  2^4
    S = SIGN
****************************************************************************/
uint16_t vReadTemp(const uint8_t u64Rom_Address[8]){

    uint16_t u16TempRaw = 0xFFFF;
    int8_t u8slide;

    if(Reset_Pulse())
    {
        if(/*u64Rom_Address == BROADCAST*/ 0) // If there are two or more sensor the data will be corrupted.
        {
            byte = 0xCC;                                  // Issue SKIP_ROM
            vWrite_Byte(byte);
        }
        else
        {
            byte = 0x55;                            // Match ROM
            vWrite_Byte(byte);

            for(u8slide = 0; u8slide < 8; u8slide++)
            {
                vWrite_Byte(u64Rom_Address[u8slide]);
//                delay_one_wire(2);
            }
        }

        byte = 0xBE;                                  //Issue READ_SCRATCHPAD
        vWrite_Byte(byte);

        u16TempRaw = Read_Byte() & 0xFF;     //  Temperature LSB
        u16TempRaw |= (Read_Byte() & 0xFF) << 8 ;     //  Temperature MSB

    }

    return (u16TempRaw);
}
/****************************************************************************
Name: Read_Rom
Function: Read 64-bit rom address.
Return: 8Bytes
***************************************************************************/
void vReadRom(uint8_t * u8Address)
{
    if(Reset_Pulse())
    {
        uint8_t i;

        byte = 0x33;
        vWrite_Byte(byte);

        for(i = 0; i < 8; i++){
            u8Address[i] = Read_Byte() & 0xFF;
        }
    }
}
/****************************************************************************
Name: Read_byte(bits)
Function: Read byte from 1-wire bus.
Return: byte
***************************************************************************/
uint8_t Read_Byte(void)
{
    uint8_t loop, result=0;

    for (loop = 0; loop < 8; loop++)
    {
        result >>= 1;               // shift the result to get it ready for the next bit to receive
        if (Read_bit())
        result |= 0x80;             // if result is one, then set MS-bit
    }

    return result;
}
/****************************************************************************

Name: Read_bit(bits)

Function: Read bit from 1-wire bus.

Return: byte

****************************************************************************/
uint8_t Read_bit(void)
{
    uint8_t read_data=0;

    PIN_setPortOutputValue(hDqPin, GPIO29_LOW);

    delay_one_wire(2);

    vInputMode();

    delay_one_wire(10);

    read_data = PIN_getInputValue(Board_DIO29_ANALOG);

    delay_one_wire(20);

    vOutputMode();

    PIN_setPortOutputValue(hDqPin, GPIO29_HIGH);

    return(read_data);
}
/****************************************************************************
Name: vWrite_Byte
Function: Escribe el Byte "byte" en el bus 1-wire
Return: Nada
****************************************************************************/
void vWrite_Byte(int byte)
{
    int j,temp;

    for(j=0;j<8;j++)
    {
        temp  = byte>>j;        //shifting "i" bits to the left
        temp &= 0x01;           //writting only the LSB bit
        vWrite_Bit(temp);
    }
}

/****************************************************************************
Name: vWrite_Bit
Function: Escribe 1 bit en el bus 1-wire
Return:
****************************************************************************/
void vWrite_Bit(int bit_val)
{
    PIN_setPortOutputValue(hDqPin, GPIO29_LOW);

    delay_one_wire(2);

    if (bit_val==1)
    {
        PIN_setPortOutputValue(hDqPin, GPIO29_HIGH);
        delay_one_wire(58);
    }else
    {
        delay_one_wire(60);
    }

    PIN_setPortOutputValue(hDqPin, GPIO29_HIGH);

    delay_one_wire(2);
}

/****************************************************************************
Name:     Reset Pulse
Function:    reset and presence detector.
Return:   TRUE: if there are sensor

            FALSE: if there aren't sensor.
****************************************************************************/
bool Reset_Pulse(void)
{
    int status = 0;

    PIN_setPortOutputValue(hDqPin, GPIO29_HIGH);        //DIO_29 High

    PIN_setPortOutputValue(hDqPin, GPIO29_LOW);

//    usleep(500);

    vInputMode();

    delay_one_wire(45);

    if(PIN_getInputValue(Board_DIO29_ANALOG))
        status=0;

    else
        status=0;

//    usleep(420);                        // wait to end

    vOutputMode();

    PIN_setPortOutputValue(hDqPin, GPIO29_HIGH);

    return(status);
}

/****************************************************************************
Name:     vInputMode
Function:    Change to input the direction of object.
Return:
****************************************************************************/
void vInputMode(void){

    PIN_close(hDqPin);
    hDqPin = PIN_open(&pinState, GPIO_InLine);
}

/****************************************************************************
Name:     vOutputMode
Function:    Change to output the direction of object.
return:
****************************************************************************/
void vOutputMode(void){

    PIN_close(hDqPin);
    hDqPin = PIN_open(&pinState, GPIO_OutLine);
}

/*---------------------------------------------------------------------------*/
//static one_wire_Handle   one_wire_handle;
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

    bool status = 0;

    int rv = 0;
        switch(type) {

            case SENSORS_HW_INIT:
                status = 1;
                if(status) {
                        vInit_OneWire();
                        temp_ds18b20.status = TEMP_DS18B20_STATUS_STANDBY;
                } else {
                  temp_ds18b20.status = TEMP_DS18B20_STATUS_DISABLED;
                  rv = TEMP_DS18B20_READING_ERROR;
                }
                break;

            case SENSORS_ACTIVE:
                if(enable) {
                  rv = TEMP_DS18B20_STATUS_ACTIVE;
                  temp_ds18b20.status = TEMP_DS18B20_STATUS_BOOTING;
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

  return temp_ds18b20.status;
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
    uint16_t res;

    (void)type;

    if(temp_ds18b20.status != TEMP_DS18B20_STATUS_DATA_READY) {
            return TEMP_DS18B20_READING_ERROR;
    }

    res = vReadTemp(read_byte);

    if (res) {
           PRINTF("EXTERNAL TEMP: %d \n", res);
    } else {
        temp_ds18b20.status = TEMP_DS18B20_STATUS_ONE_WIRE_ERROR;
        return TEMP_DS18B20_READING_ERROR;
    }

    return (int)res;
}
/*-------------------------------------------------------------------------------*/
SENSORS_SENSOR(temp_ds18b20_sensor, "TEMP-DS18B20-SENSOR", value, configure, status);
/*-------------------------------------------------------------------------------*/
/** @} */
