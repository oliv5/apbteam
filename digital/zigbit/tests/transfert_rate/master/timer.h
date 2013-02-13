/* timer.h */
/* Timer ms. {{{
 *
 * Copyright (C) 2013 Florent Duchon
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

#include <appTimer.h>
#define COUNT_TIMER_GRANULARITY			100L

/* Timer callback called each 100 ms */
void timer_callback(void);

/* Timer start */
void timer_start(void);

/* Timer stop */
void timer_stop(void);

/* This function returns the timer value in seconds */
float timer_get_value_s(void);
