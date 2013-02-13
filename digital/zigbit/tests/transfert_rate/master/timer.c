/* timer.c */
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

#include "timer.h"
#include "print.h"

static HAL_AppTimer_t timerconf;

uint16_t timer_value = 0;

/* Timer callback called each 100 ms */
void timer_callback(void)
{
	timer_value++;
}

/* Timer start */
void timer_start(void)
{
	timerconf.interval = COUNT_TIMER_GRANULARITY;
	timerconf.mode     = TIMER_REPEAT_MODE;
	timerconf.callback = timer_callback;
	timer_value = 0;
	HAL_StartAppTimer(&timerconf);
}

/* Timer stop */
void timer_stop(void)
{
	HAL_StopAppTimer(&timerconf);
}

/* This function returns the timer value in seconds */
float timer_get_value_s(void)
{
	return (float)timer_value/(float)10;
}



