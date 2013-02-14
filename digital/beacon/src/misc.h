/* misc.h */
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

#ifndef _MISC_H
#define _MISC_H

#define COLOR_LEFT 1
#define COLOR_RIGHT 0

/* This function simulates the jack's state and send it over the air */
void jack_on_off(void);

/* This function sends the jack status to the slave beacons*/
void jack_update_status(uint8_t value);

void color_set_value(uint8_t value);

uint8_t color_get_value(void);



#endif
