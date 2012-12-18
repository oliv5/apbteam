/* trust.c */
/* Beacon Trust control. {{{
 *
 * Copyright (C) 2011 Florent Duchon
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

#include "position.h"
#include "trust.h"
#include "debug_simu.h"
#include "debug_avr.h"

extern opponent_s opponent[MAX_OBSTACLE];
static HAL_AppTimer_t trustTimer;


/* This function returns the trust level */
TTrustStatus trust_check_level(void)
{
	int i = 0;
	for( i = 1; i <= MAX_OBSTACLE ; i++)
	{
		/* If at least one obstacle is under the trust threashold, we are not confident about its position */
		DEBUG_TRUST("opponent[%d].trust == %d\n",i,opponent[i].trust);
		if(opponent[i].trust < TRUST_THRESHOLD)
		{
 			return TRUST_TOO_BAD;
		}
	}
	return TRUST_LEVEL_OK;
}


/* This function increases the trust level for a specifical obstacle */
TTrustStatus trust_increase(int number)
{
	if(opponent[number].trust < TRUST_MAX)
	{
		opponent[number].trust++;
	}
	return TRUST_LEVEL_OK;
}

/* This function decreases the trust level for all obstacles */
TTrustStatus trust_decrease(void)
{
	int i = 0;
	for( i = 1; i <= MAX_OBSTACLE ; i++)
	{
		if(opponent[i].trust > TRUST_MIN)
		{
 			opponent[i].trust--;
		}
	}
	
	return TRUST_LEVEL_OK;
}


void trust_decrease_task(void)
{
	trustTimer.interval = TRUST_DECREASE_TASK_PERIOD;
	trustTimer.mode     = TIMER_REPEAT_MODE;
	trustTimer.callback = trust_decrease;
	HAL_StartAppTimer(&trustTimer);
}

