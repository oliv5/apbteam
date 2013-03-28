/* recovery.c */
/* Beacon recovery mode. {{{
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
#include "recovery.h"
#include "trust.h"
#include "debug_simu.h"

/* This function is used to calculate all obstacle positions from sractch */
TRecoveryStatus recovery(coord_s * new_point,robot_s opp[MAX_OBSTACLE])
{
	/* Declare variables */
	int i = 0;
	int j = 0;
	int dx = 0;
	int dy = 0;

	static int counter = 0;								/* Loop counter */
	static int recovery_number = 0;						/* Potential valid positions Number */
	static int best_occurence = 0;							/* Maximum occurence amoung Potential valid Position */
	static recovery_s recovery_tab[MAX_POINT_RECOVERY];	/* Potention valid position Table containing (x;y) and occurence for all positions found */
	
	int best_position = 0;								/* Best Position variable found into recovery tab */

	
	/* In recovery mode, trust must be lowest possible till real position is not found. */
	for(j = 1 ; j <= MAX_OBSTACLE ; j++)
	{
		opp[j].trust=TRUST_MIN;
	}

	/* If we didn't reach the recovery threshold we continue to feed the recovery tab */	
	if(counter < MAX_POINT_RECOVERY)
	{	
		/* Search into already known positions */
		for(i = 0; i < recovery_number ; i ++)
		{
			dx = recovery_tab[i].x - new_point->x;
			dy = recovery_tab[i].y - new_point->y;
			
			if(dx * dx + dy * dy < OBSTACLE_RADIUS * OBSTACLE_RADIUS)
			{
				/* Seems be this position : update structure*/
				recovery_tab[i].occurence++;
				recovery_tab[i].x = new_point->x;
				recovery_tab[i].y = new_point->y;
				counter++;
				
				/* Check if the new occurence is the best one amout all other positions */
				if(recovery_tab[i].occurence > best_occurence)
				{
					best_occurence = recovery_tab[i].occurence;
					/* !! Tricky operation  !! : If counter reaches a defined threshold and if we just found the best occurence (ie best accuracy) we invalidate the current angle in order to ignore it next time.  */
					/* Doing that increase a lot the global computation accuracy by  ignoring unwanted phantom position */
					if(counter > RECOVERY_TRICKY_THRESHOLD)
					{ 				
						return RECOVERY_IGNORE_ANGLE_NEXT_TIME;
					}
				}
				/* Value found, counter and structures updated. Return NO_ERROR */
				return RECOVERY_IN_PROGRESS;
				
			}
		}
		/* Position was not found in the tab, we have to add it */
		recovery_tab[recovery_number].x = new_point->x;
		recovery_tab[recovery_number].y = new_point->y;
		recovery_tab[recovery_number].occurence++;
		recovery_number++;
		counter++;
		return RECOVERY_IN_PROGRESS;
	}
	else /* We have sufficient values to find absolute position of all obstacles */
	{
		
		for(j = 0 ; j < recovery_number ; j++)
		{
 			DEBUG_RECOVERY("[%d] (%d ; %d)\n",recovery_tab[j].occurence,recovery_tab[j].x,recovery_tab[j].y);
		}
		
		for(i = 1 ; i <= MAX_OBSTACLE ; i++)
		{
			/* Search for the position with best occurence */
			for(j = 0 ; j < recovery_number ; j++)
			{
				if(recovery_tab[j].occurence >= best_occurence)
				{
					best_position = j;
					best_occurence = recovery_tab[j].occurence;
				}
			}
			
			/* Update the obstacle structure with update position */
			recovery_tab[best_position].occurence = 0;
			opp[i].x = recovery_tab[best_position].x;
			opp[i].y = recovery_tab[best_position].y;
			opp[i].trust = TRUST_MAX;
			
			/* Reset variable for next obstacle */
			best_position = 0;
			best_occurence = 0;
			DEBUG_RECOVERY("Opponent %d ( %d ; %d ) trust = %d\n",i,opp[i].x,opp[i].y,opp[i].trust);
		}
		
		/* Reset couner variable for next recovery */
		counter = 0;
		recovery_number = 0;
		best_occurence = 0;
		for(j = 0 ; j < MAX_POINT_RECOVERY ; j++)
		{
			recovery_tab[j].occurence=0;
			recovery_tab[j].x=0;
			recovery_tab[j].y=0;
		}
		return RECOVERY_FINISHED;
	}
}


