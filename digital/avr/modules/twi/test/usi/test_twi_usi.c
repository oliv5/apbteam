/* test_twi_usi.c */
/* avr.twi - TWI AVR module. {{{
 *
 * Copyright (C) 2010 Nicolas Schodet
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
#include "common.h"
#include "modules/twi/twi.h"
#include "io.h"

int
main (int argc, char **argv)
{
    /* Initialize TWI. */
    twi_init (0x04);
    while (42)
      {
	/* Simulate interrupts. */
	twi_update ();
	/* Check for data. */
	uint8_t data[AC_TWI_SLAVE_RECV_BUFFER_SIZE];
	uint8_t data_len;
	data_len = twi_slave_poll (data, AC_TWI_SLAVE_RECV_BUFFER_SIZE);
	if (data_len)
	  {
	    /* Echo them back.*/
	    twi_slave_update (data, data_len);
	  }
      }
}

