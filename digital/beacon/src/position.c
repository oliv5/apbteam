/* position.c */
/* Beacon triangulation algorithms. {{{
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

#include <stdint.h>
#include "position.h"
#include "debug_simu.h"
#include "recovery.h"
#include "update.h"
#include "formula.h"
#include "trust.h"

beacon_s beacon[MAX_BEACON+1];
opponent_s opponent[MAX_OBSTACLE+1];

/* This function is used to initialize all needed structures */
void position_init_struct(void)
{
	int i = 0;
	int j = 0;
	for(i = 1; i <= MAX_BEACON; i++)
	{
		beacon[i].angleNumber = 0;
		for(j = 1; j <= MAX_OBSTACLE ; j++)
		{
			beacon[i].angle[j] = 1;
		}
	}
	
	for(i = 1; i <= MAX_OBSTACLE; i++)
	{
		opponent[i].x = 0;
		opponent[i].y = 0;
		opponent[i].trust = TRUST_MIN;
	}
}

/* This function update the opponent position when a new angle is avalaible */
int update_position(uint16_t beaconID, uint16_t angleID, float angle)
{
	static uint16_t last_ID[2] = {0};
	uint16_t last_valid_id = 0;
	uint16_t which_formula = 0;	
	coord_s temp_position[MAX_TEMP_POSITION];
	uint16_t i = 0;
	
	uint16_t formula_status = 0;
	uint16_t global_status = 0;
	uint16_t update_status = UPDATE_OBSTACLE_NOT_FOUND;
	uint16_t recovery_status = 0;
	
	DEBUG_POSITION("Update_position with beaconID = %d and angleID = %d and angle = %f\n",(int)beaconID,(int) angleID, (double)angle);
	DEBUG_POSITION("last_ID[0]  = %d  last_ID[1]  = %d\n",(int)last_ID[0],(int)last_ID[1]);

	/* Calculate which formula need to be used to compute position */
	for(i = 0 ; i < 2; i++)
	{
		if(beaconID != last_ID[i])
		{
			last_valid_id = last_ID[i];
		}
	}
	which_formula = beaconID + last_valid_id;
		

	if(last_valid_id != 0)
	{
		if(trust_check_level() == TRUST_LEVEL_OK)
		{
			/* Compute all hypotheticals positions and save them into temporary_position tab*/
			for(i = 1 ; i <= beacon[last_valid_id].angleNumber ; i++)
			{
				formula_status = formula_compute_position(which_formula,beaconID,beacon[last_valid_id].angle[i],angle,&temp_position[i]);
				if(formula_status == FORMULA_VALID_POSITION)
				{
					update_status += update(&temp_position[i]);
					if(update_status == UPDATE_OBSTACLE_FOUND)
					{
						break;
					}
				}
			}
			if(update_status == UPDATE_OBSTACLE_NOT_FOUND)
			{
				/* Obstacle not found */
				trust_decrease();
			}
			global_status = POSITION_NO_ERROR;
		}
		else /* Need Recovery */
		{
			/* Compute all hypotheticals positions and save them into temporary_position tab*/
			for(i = 1 ; i <= beacon[last_valid_id].angleNumber ; i++)
			{				
				if(beacon[last_valid_id].angle[i] != IGNORE_ANGLE)
				{
					formula_status = formula_compute_position(which_formula,beaconID,beacon[last_valid_id].angle[i],angle,&temp_position[i]);
					if(formula_status == FORMULA_VALID_POSITION)
					{
						/* If the angle is not ignored and the computed position is valid, feed the recovery system */
						recovery_status = recovery(&temp_position[i],&opponent[0]);
						if((recovery_status == RECOVERY_IN_PROGRESS)||(recovery_status == RECOVERY_FINISHED))
						{
							global_status = POSITION_NO_ERROR;
							break;
						}
						else if(recovery_status == RECOVERY_IGNORE_ANGLE_NEXT_TIME)
						{
							global_status = POSITION_IGNORE_ANGLE;
							break;
						}
					}
				}
			}
		}
	}

	/* Save angle context */
	beacon[beaconID].angleNumber = angleID;
	if(global_status == POSITION_NO_ERROR)
	{
		beacon[beaconID].angle[angleID] = angle;
	}
	else /* Angle must be ignored next time */
	{
		beacon[beaconID].angle[angleID] = IGNORE_ANGLE;
	}

	/* Save ID context */
	if(beaconID != last_valid_id)
	{
		last_ID[1] = last_ID[0];
		last_ID[0] = beaconID;
	}
	return 0;
}

/* This function returns the requested coord according to the opponent number */
int16_t position_get_coord(TOpponent_ID id, TCoord_type type)
{
	switch(type)
	{
		case X:
			return opponent[id].x;
			break;
		case Y:
			return opponent[id].y;
			break;
		default:
			return 0;
	}
}

/* This function returns the trust according to opponent number */
int8_t position_get_trust(TOpponent_ID id)
{
	return opponent[id].trust;
}