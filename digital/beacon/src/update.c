/* update.c */
/* Beacon udapte position mode. {{{
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

#include "position.h"
#include "trust.h"
#include "update.h"
#include "debug_simu.h"
#include "print.h"

/* Globals Declaration */
extern robot_s opponent[MAX_OBSTACLE];

/* This function checks is the given coord is a potential obstacle and updates the structure in consequence */
TUpdateStatus update(coord_s * point)
{
	int j = 0;
	int dx = 0;
	int dy = 0;
	
	/* Find if it's near a previous obstacle */
	DEBUG_UPDATE("Check if ( %.4d ; %.4d ) exists\n",point->x,point->y); 
	
	for( j = 1; j <= MAX_OBSTACLE ; j++)
	{
		dx = opponent[j].x - point->x;
		dy = opponent[j].y - point->y;
		if (dx * dx + dy * dy < OBSTACLE_RADIUS * OBSTACLE_RADIUS)
		{
// 			uprintf("Opponent found (%.4d ; %.4d)\r\n",opponent[j].x,opponent[j].y);
			opponent[j].x = (opponent[j].x + point->x)/2;
			opponent[j].y = (opponent[j].y + point->y)/2;
			trust_increase(j);
			uprintf("%.4d;%.4d;%d\r\n",opponent[j].x,opponent[j].y,opponent[j].trust);
			return UPDATE_OBSTACLE_FOUND;
		}
	}
	
	/* No obstacle found */
	DEBUG_UPDATE("Opponent not found\n");
	return UPDATE_OBSTACLE_NOT_FOUND;
}


