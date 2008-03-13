/* test_counter.c */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2008 Nicolas Schodet
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
#include "modules/utils/utils.h"
#include "modules/uart/uart.h"
#include "modules/proto/proto.h"

#include "timer.h"
#include "misc.h"

#include "counter_ext.avr.c"

uint8_t read, read_cpt;
uint8_t ind, ind_cpt, ind_init;
uint8_t count, count_cpt;

int
main (void)
{
    uint8_t old_ind = 0;
    const int total = 5000;
    LED_SETUP;
    timer_init ();
    counter_init ();
    uart0_init ();
    proto_send0 ('z');
    sei ();
    while (1)
      {
	timer_wait ();
	if (count)
	    counter_update ();
	if (read && !--read_cpt)
	  {
	    proto_send2b ('r', counter_read (0), counter_read (3));
	    read_cpt = read;
	  }
	if (ind && !--ind_cpt)
	  {
	    uint8_t i = counter_read (3);
	    if (!ind_init && i != old_ind)
	      {
		uint8_t eip = old_ind + total;
		uint8_t eim = old_ind - total;
		proto_send7b ('i', old_ind, i, eip, eim, i - eip, i - eim,
			      i == eip || i == eim);
	      }
	    old_ind = i;
	    ind_init = 0;
	    ind_cpt = ind;
	  }
	if (count && !--count_cpt)
	  {
	    proto_send3w ('C', counter_left, counter_right, counter_aux0);
	    count_cpt = count;
	  }
	while (uart0_poll ())
	    proto_accept (uart0_getc ());
      }
}

/** Handle incoming messages. */
void
proto_callback (uint8_t cmd, uint8_t size, uint8_t *args)
{
#define c(cmd, size) (cmd << 8 | size)
    switch (c (cmd, size))
      {
      case c ('z', 0):
	/* Reset. */
	utils_reset ();
	break;
      case c ('r', 1):
	read_cpt = read = args[0];
	break;
      case c ('i', 1):
	ind_cpt = ind = args[0];
	ind_init = 1;
	break;
      case c ('C', 1):
	count_cpt = count = args[0];
	break;
      default:
	proto_send0 ('?');
	return;
      }
    proto_send (cmd, size, args);
#undef c
}
