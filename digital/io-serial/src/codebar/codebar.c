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
#include "modules/twi/twi.h"
#include "modules/proto/proto.h"
#include "modules/uart/uart.h"
#include "modules/utils/utils.h"
#include "modules/utils/byte.h"
#include "modules/utils/crc.h"
#include "timer.h"

/* from robospierre/element.h file */
#define ELEMENT_UNKOWN 0
#define ELEMENT_QUEEN  4
#define ELEMENT_KING   8

#define STRING_MAX      10

struct status_t
{
    uint16_t age1;
    uint8_t piece1;
    uint16_t age2;
    uint8_t piece2;
};

char buffer_u0[STRING_MAX], buffer_u1[STRING_MAX];

uint8_t
string_to_element (char *data, uint8_t data_len)
{
    uint8_t i;
    for (i = 0; i < data_len - 5; i++)
      {
	if (memcmp (data + i, "QUEEN", 5) == 0)
	  {
	    data[i] = 0;
	    return ELEMENT_QUEEN;
	  }
      }
    for (i = 0; i < data_len - 4; i++)
      {
	if (memcmp (data + i, "KING", 4) == 0)
	  {
	    data[i] = 0;
	    return ELEMENT_KING;
	  }
      }
    return ELEMENT_UNKOWN;
}

void
read_strings (void)
{
    uint8_t i;
    while (uart0_poll ())
      {
	/* Insert char at end of string. */
	for (i = 1; i < STRING_MAX; i++)
	    buffer_u0[i - 1] = buffer_u0[i];
	buffer_u0[STRING_MAX - 1] = uart0_getc ();
      }
    while (uart1_poll ())
      {
	/* Insert char at end of string. */
	for (i = 1; i < STRING_MAX; i++)
	    buffer_u1[i - 1] = buffer_u1[i];
	buffer_u1[STRING_MAX - 1] = uart1_getc ();
      }
}

int
main (int argc, char **argv)
{
    struct status_t status;
    uint8_t element_type;

    status.age1   = 0;
    status.piece1 = ELEMENT_UNKOWN;
    status.age2   = 0;
    status.piece2 = ELEMENT_UNKOWN;

    timer_init ();
    avr_init (argc, argv);
    sei ();
    uart0_init ();
    uart1_init ();
    /* Initialize TWI. */
    twi_init (0x20);

    while (1)
      {
	/* Wait until next cycle. */
	timer_wait ();
	if (status.age1 < (uint16_t) -1)
	    status.age1 ++;
	if (status.age2 < (uint16_t) -1)
	    status.age2 ++;

	read_strings ();
	element_type = string_to_element (buffer_u0, STRING_MAX);
	if (element_type)
	  {
	    status.piece1 = element_type;
	    status.age1 = 0;
	  }
	element_type = string_to_element (buffer_u1, STRING_MAX);
	if (element_type)
	  {
	    status.piece2 = element_type;
	    status.age2 = 0;
	  }

	uint8_t status_with_crc[7];
	uint8_t *status_twi = &status_with_crc[1];
	status_twi[0] = v16_to_v8 (status.age1, 1);
	status_twi[1] = v16_to_v8 (status.age1, 0);
	status_twi[2] = status.piece1;
	status_twi[3] = v16_to_v8 (status.age2, 1);
	status_twi[4] = v16_to_v8 (status.age2, 0);
	status_twi[5] = status.piece2;
	/* Compute CRC. */
	status_with_crc[0] = crc_compute (&status_with_crc[1], sizeof (status_with_crc) - 1);
	twi_slave_update (status_with_crc, sizeof (status_with_crc));
      }
    return 0;
}
