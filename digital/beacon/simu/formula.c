/* formula.c */
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

#include <math.h>
#include "debug.h"
#include "position.h"
#include "formula.h"

/* This function computes coords using formula 3 mode ie for beacon 1 + 2*/
TFormulaStatus formula3_compute_coord(coord_s * position, float angle_beacon1, float angle_beacon2)
{
 	DEBUG_FORMULA_INFO("Formula 3 : angle_beacon1 = %f angle_beacon2 = %f\n",angle_beacon1,angle_beacon2);
	if((angle_beacon1 < BLIND_ZONE_BEACON1_ANGLE12_MAX) && (angle_beacon2 < BLIND_ZONE_BEACON2_ANGLE12_MAX))
	{
		DEBUG_FORMULA_ERROR("BLIND Zone detected\n");
		return FORMULA_BLIND_ZONE_DETECTED;
	}
	else
	{
		position->x = LARGEUR_TABLE * tan(angle_beacon2) * tan(angle_beacon1) / (tan(angle_beacon2)+tan(angle_beacon1));
 		position->y = LARGEUR_TABLE * tan(angle_beacon1) / (tan(angle_beacon2)+tan(angle_beacon1));
		
		if( (position->x < 0) || (position->x > LONGUEUR_TABLE) || (position->y < 0) || (position->y > LARGEUR_TABLE))
		{
			DEBUG_FORMULA_ERROR("OUT of the Table\n");
			return FORMULA_OUT_OF_TABLE;
		}
		else
		{
			DEBUG_FORMULA_INFO("Formula 3 : X = %d  Y = %d\n",position->x,position->y);
			return FORMULA_VALID_POSITION;
		}
	}
}

/* This function computes coords using formula 4 mode ie for beacon 1 + 3*/
TFormulaStatus formula4_compute_coord(coord_s * position, float angle_beacon1, float angle_beacon3)
{
 	DEBUG_FORMULA_INFO("Formula 4 : angle_beacon1 = %f angle_beacon3 = %f\n",angle_beacon1,angle_beacon3);

	if(angle_beacon3 > M_PI/2)
	{
		if((angle_beacon1 > BLIND_ZONE_BEACON1_ANGLE13_MIN) &&  (angle_beacon1 < BLIND_ZONE_BEACON1_ANGLE13_MAX) && ((M_PI - angle_beacon3) > BLIND_ZONE_BEACON3_ANGLE13_MIN) && ((M_PI - angle_beacon3) < BLIND_ZONE_BEACON3_ANGLE13_MAX))
		{
			DEBUG_FORMULA_ERROR("BLIND Zone detected\n");
			return FORMULA_BLIND_ZONE_DETECTED;
		}
		position->y = (LARGEUR_DEMI_TABLE*tan(M_PI - angle_beacon3) - LARGEUR_TABLE*tan(angle_beacon1) + LONGUEUR_TABLE) / (tan(M_PI - angle_beacon3) - tan(angle_beacon1));
	}
	else
	{
		position->y = (LARGEUR_DEMI_TABLE*tan(angle_beacon3) + LARGEUR_TABLE*tan(angle_beacon1)-LONGUEUR_TABLE) / (tan(angle_beacon1) + tan(angle_beacon3));
	}
	position->x = (LARGEUR_TABLE - position->y)*tan(angle_beacon1);
	
	if( (position->x < 0) || (position->x > LONGUEUR_TABLE) || (position->y < 0) || (position->y > LARGEUR_TABLE))
	{
		DEBUG_FORMULA_ERROR("OUT of the Table\n");
		return FORMULA_OUT_OF_TABLE;
	}
	else
	{
		DEBUG_FORMULA_INFO("Formula 4 : X = %d  Y = %d\n",position->x,position->y);
		return FORMULA_VALID_POSITION;
	}
}

/* This function computes coords using formula 5 mode ie for beacon 2 + 3*/
TFormulaStatus formula5_compute_coord(coord_s * position, float angle_beacon2, float angle_beacon3)
{
	DEBUG_FORMULA_INFO("Formula 5 : angle_beacon2 = %f angle_beacon3 = %f\n",angle_beacon2,angle_beacon3);
	
	if(angle_beacon3 > M_PI/2)
	{
		position->y = (LONGUEUR_TABLE + LARGEUR_DEMI_TABLE * tan(M_PI - angle_beacon3)) / (tan(angle_beacon2) + tan(M_PI - angle_beacon3));
	}
	else
	{
		if((angle_beacon2 > BLIND_ZONE_BEACON2_ANGLE23_MIN) &&  (angle_beacon2 < BLIND_ZONE_BEACON2_ANGLE23_MAX) && (angle_beacon3 > BLIND_ZONE_BEACON3_ANGLE23_MIN) && (angle_beacon3 < BLIND_ZONE_BEACON3_ANGLE23_MAX))
		{
			DEBUG_FORMULA_ERROR("BLIND Zone detected\n");
			return FORMULA_BLIND_ZONE_DETECTED;
		}
		position->y = (LARGEUR_DEMI_TABLE*tan(angle_beacon3) - LONGUEUR_TABLE) / (tan(angle_beacon3) - tan(angle_beacon2));
	}
	position->x = tan(angle_beacon2) * position->y;
	
	if( (position->x < 0) || (position->x > LONGUEUR_TABLE) || (position->y < 0) || (position->y > LARGEUR_TABLE))
	{
		DEBUG_FORMULA_ERROR("OUT of the Table\n");
		return FORMULA_OUT_OF_TABLE;
	}
	else
	{
		DEBUG_FORMULA_INFO("Formula 5 : X = %d  Y = %d\n",position->x,position->y);
		return FORMULA_VALID_POSITION;
	}
}


/* This function computes an absolute position using 2 angles from 2 beacons */
TFormulaStatus formula_compute_position(int formula, int current_beacon_ID, float latest_angle, float current_angle, coord_s * result)
{
	int error = FORMULA_VALID_POSITION;
	switch(formula)
	{
		case 3:
			if(current_beacon_ID == 1)
			{
				error = formula3_compute_coord(result, current_angle, latest_angle);
			}
			else
			{
				error = formula3_compute_coord(result, latest_angle, current_angle);
			}
			break;
		case 4:
			if(current_beacon_ID == 1)
			{
				error = formula4_compute_coord(result, current_angle, latest_angle);
			}
			else
			{
				error = formula4_compute_coord(result, latest_angle, current_angle);
			}
			break;
		case 5:
			if(current_beacon_ID == 2)
			{
				error = formula5_compute_coord(result, current_angle,latest_angle);
			}
			else
			{
				error = formula5_compute_coord(result, latest_angle, current_angle);
			}
			break;
		default:
			DEBUG_FORMULA_ERROR("Unknown Formula = %d\r\n",formula);
			error = FORMULA_UNKNOWN_FORMULA;
	}
	return error;
}


