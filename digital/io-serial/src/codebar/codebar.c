/* codebar.c */
/* codebar - Codebar Reader. {{{
 *
 * Copyright (C) 2011
 *
 * APBTeam:
 *        Web: http://apbteam.org/
 *      Email: team AT apbteam DOT org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * }}} */

#include <stdio.h>                                                                                                                          
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "codebar.h"
#include "modules/twi/twi.h"
#include "modules/proto/proto.h"
#include "modules/uart/uart.h"
#include "modules/utils/utils.h"
#include "modules/utils/byte.h"
#include "modules/utils/crc.h"
#include "timer.h"

/* from robospierre/element.h file */
#define ELEMENT_UNKOWN 0
#define ELEMENT_PAWN   1
#define ELEMENT_QUEEN  2
#define ELEMENT_KING   4

#define KING  "KING"
#define QUEEN "QUEEN"
#define PAWN  "PAWN"

#define STRING_MAX      10

struct status_t
{
    uint16_t age1;
    uint8_t piece1;
    uint16_t age2;
    uint8_t piece2;
};

void proto_callback (uint8_t cmd, uint8_t size, uint8_t *args)
{
    /* nothing here*/
}

uint8_t string_to_element(char* data)
{
    if (strlen(data) == 5)
        return ELEMENT_QUEEN;
    if (strcmp(data, KING) == 0)
        return ELEMENT_KING;
    if (strcmp(data, PAWN) == 0)
        return ELEMENT_PAWN;
    return ELEMENT_UNKOWN;
}

char* read_string(int uart_port)
{ 
    static int cnt_char_u0 = 0;
    static char buffer_u0[STRING_MAX] = {'\0'};
    static int cnt_char_u1 = 0;
    static char buffer_u1[STRING_MAX] = {'\0'};

    char c;

    if (uart_port == 0 && uart0_poll ())
    {
        while ( uart0_poll() && (c = uart0_getc()) != '\r')
        {
            buffer_u0[cnt_char_u0] = c;
            cnt_char_u0++;
        }
        buffer_u0[cnt_char_u0+1] = '\0';
        cnt_char_u0 = 0;
        return buffer_u0;
    }
    else if (uart_port == 1 && uart1_poll()) 
    {
        while ( uart1_poll() && (c = uart1_getc()) != '\r')
        {
            buffer_u1[cnt_char_u1] = c;
            cnt_char_u1++;
        }
        buffer_u1[cnt_char_u1+1] = '\0';
        cnt_char_u1 = 0;
        return buffer_u1;
    }
    else
    {
        return NULL;
    }
}

int
main (int argc, char **argv)
{
    char* buffer;
    struct status_t status;

    status.age1   = 0;
    status.piece1 = ELEMENT_UNKOWN;
    status.age2   = 0;
    status.piece2 = ELEMENT_UNKOWN;

    timer_init ();
    avr_init (argc, argv);
    sei ();
    uart0_init ();
    uart1_init ();
    /* We have successfully boot. */
    proto_send0 ('z');
    /* Initialize TWI. */
    twi_init (0x04);
    /* I am a slave. */
    proto_send0 ('S');
	
    while (1)
    {
        /* Wait until next cycle. */
    	timer_wait ();
        if (status.age1 < (uint16_t) -1)
            status.age1 ++;
        if (status.age2 < (uint16_t) -1)
            status.age2 ++;

        if ((buffer = read_string(0)) != NULL)
        { 
            status.piece1 = string_to_element(buffer);
        }

        if ((buffer = read_string(1)) != NULL)
        { 
            status.piece2 = string_to_element(buffer);
        }

        uint8_t status_with_crc[8];
        uint8_t *status_twi = &status_with_crc[1];
        status_twi[0] = v16_to_v8 (status.age1, 0);
        status_twi[1] = v16_to_v8 (status.age1, 1);
        status_twi[2] = status.piece1;
        status_twi[3] = v16_to_v8 (status.age2, 0);
        status_twi[4] = v16_to_v8 (status.age2, 1);
        status_twi[5] = status.piece2;
        status_twi[6] = 42;
        status_twi[7] = 32;
        /* Compute CRC. */
        status_with_crc[0] = crc_compute (&status_with_crc[1], sizeof (status_with_crc) - 1);
        twi_slave_update (status_with_crc, sizeof (status_with_crc));

    }
    return 0;
}
