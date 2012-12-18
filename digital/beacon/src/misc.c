/* misc.c */
/* miscellaneous function. {{{
 *
 * Copyright (C) 2012 Florent Duchon
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

#include "misc.h"
#include "network.h"

static int8_t color = -1;
/* This function resets the avr using the embedded watchdog */
void reset_avr(void)
{
	wdt_enable (WDTO_15MS);
	while (1);
}

/* This function simulates the jack's state and send it over the air */
void jack_on_off(void)
{
	static bool jack = 0;
	if(jack == 0)
		jack = 1;
	else
		jack = 0;
	network_send_data(NETWORK_JACK_STATE,jack);
}


/* This function sends the jack status to the slave beacons*/
void jack_update_status(uint8_t value)
{
	uint8_t old_jack = 0;
	if(value != old_jack)
	{
		network_send_data(NETWORK_JACK_STATE,value);
		old_jack = value;
	}	
}

void color_set_value(uint8_t value)
{
	color = value;
}

uint8_t color_get_value(void)
{
	return color;
}
