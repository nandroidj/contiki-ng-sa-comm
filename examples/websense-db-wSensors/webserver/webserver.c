/*
 * Copyright (c) 2017, RISE SICS
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

#include "contiki.h"
#include "net/routing/routing.h"
#include "net/ipv6/uip-ds6-nbr.h"
#include "net/ipv6/uip-ds6-route.h"
#include "net/ipv6/uip-sr.h"

#include "sensing.h"

#include "net/ipv6/uip.h"
#include "net/ipv6/uiplib.h"
#include "net/ipv6/ip64-addr.h"

#include <stdio.h>
#include <string.h>
/*---------------------------------------------------------------------------*/
int internal_temperature, external_temperature, internal_humidity, external_humidity;

/*---------------------------------------------------------------------------*/

#define ADD(...) do {                                                   \
    blen += snprintf(&buf[blen], sizeof(buf) - blen, __VA_ARGS__);      \
  } while(0)
#define SEND(s) do { \
  SEND_STRING(s, buf); \
  blen = 0; \
} while(0);

/* Use simple webserver with only one page for minimum footprint.
 * Multiple connections can result in interleaved tcp segments since
 * a single static buffer is used for all segments.
 */
#include "httpd-simple.h"

void get_ipv6_address(char * buffer, size_t size);
/*---------------------------------------------------------------------------*/
static
PT_THREAD(generate_routes(struct httpd_state *s))
{
    char buff[35];

    char buffer[UIPLIB_IPV6_MAX_STR_LEN];

    get_ipv6_address(buffer, sizeof(buffer));
    printf("%s", buffer);

    PSOCK_BEGIN(&s->sout);
    //SEND_STRING(&s->sout, TOP);

    sprintf(buff,"{\"internal_temp\":%u,\"external_temp\":%u,\"internal_hum\":%u,\"external_hum\":%u}", internal_temperature, external_temperature,internal_humidity, external_humidity);
    printf("send json to requester\n");
    SEND_STRING(&s->sout, buff);
    //SEND_STRING(&s->sout, BOTTOM);
    PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
PROCESS(webserver_nogui_process, "Web server");
PROCESS_THREAD(webserver_nogui_process, ev, data)
{
    PROCESS_BEGIN();

    httpd_init();

    init_battery_sensor();
    get_battery_sensor_reading();
    init_sensor_readings();

    while(1) {

      PROCESS_YIELD();

      if(ev == PROCESS_EVENT_TIMER) {
          get_battery_sensor_reading();
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

    while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
    httpd_appcall(data);
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
httpd_simple_script_t
httpd_simple_get_script(const char *name)
{
  return generate_routes;
}
/*---------------------------------------------------------------------------*/

void get_ipv6_address(char * buffer, size_t size) {

    uip_ds6_addr_t *lladdr;
    memcpy(&uip_lladdr.addr, &linkaddr_node_addr, sizeof(uip_lladdr.addr));
    //process_start(&tcpip_process, NULL);

    lladdr = uip_ds6_get_link_local(-1);
    uiplib_ipaddr_snprint(buffer, sizeof(buffer), &lladdr->ipaddr);
}
