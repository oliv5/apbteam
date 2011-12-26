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

#include <stdio.h>
#include "position.h"

extern beacon_value_s beacon;
extern opponent_s opponent;

void init_struct(void)
{
	beacon.angle[0]=0;
	beacon.angle[1]=0;
	beacon.angle[2]=0;
	beacon.angle[3]=0;
	beacon.last_updated_beacon = 0;
	beacon.before_last_updated_beacon = 0;
	opponent.x = 0;
	opponent.y = 0;
}

float degree_to_radian(int value)
{
	float temp = 0;
	temp = (value * 2 * M_PI) / 360;
	return temp;
}

void update_position(int beacon_number,double angle)
{
	int which_formula = 0;	
	if(beacon_number == beacon.last_updated_beacon)
	{
		beacon.last_updated_beacon = beacon_number;
	}
	else
	{
		beacon.before_last_updated_beacon = beacon.last_updated_beacon;
		beacon.last_updated_beacon = beacon_number;
	}
	which_formula = beacon.before_last_updated_beacon + beacon.last_updated_beacon;
// 	printf("[position.c] => Update_position beacon_number = %d   angle = %f\n",(int)beacon_number,(double)angle);
	beacon.angle[beacon_number] = angle;

	switch(which_formula)
	{
		case 3:
//  			printf("[position.c] => Formula 3\r\n");
// 			printf("[position.c] => angle[1] = %f angle[2] = %f\n",beacon.angle[1],beacon.angle[2]);
  			opponent.x = LARGEUR_TABLE * tan(beacon.angle[2]) * tan(beacon.angle[1]) / (tan(beacon.angle[2])+tan(beacon.angle[1]));
  			opponent.y = LARGEUR_TABLE * tan(beacon.angle[1]) / (tan(beacon.angle[2])+tan(beacon.angle[1]));
			break;
		case 4:
//  			printf("[position.c] => Formula 4\r\n");
			if(beacon.angle[3] > M_PI/2)
			{
 				opponent.y = (LARGEUR_DEMI_TABLE*tan(M_PI - beacon.angle[3]) - LARGEUR_TABLE*tan(beacon.angle[1]) + LONGUEUR_TABLE) / (tan(M_PI - beacon.angle[3]) - tan(beacon.angle[1]));
			}
			else
			{
				opponent.y = (LARGEUR_DEMI_TABLE*tan(beacon.angle[3]) + LARGEUR_TABLE*tan(beacon.angle[1])-LONGUEUR_TABLE) / (tan(beacon.angle[1]) + tan(beacon.angle[3]));
			}
			opponent.x = (LARGEUR_TABLE - opponent.y)*tan(beacon.angle[1]);
			break;
		case 5:
// 			printf("[position.c] => formula 5\r\n");
			if(beacon.angle[3] > M_PI/2)
			{
				opponent.y = (LONGUEUR_TABLE + LARGEUR_DEMI_TABLE * tan(M_PI - beacon.angle[3])) / (tan(beacon.angle[2]) + tan(M_PI - beacon.angle[3]));
			}
			else
			{
				opponent.y = (LARGEUR_DEMI_TABLE*tan(beacon.angle[3]) - LONGUEUR_TABLE) / (tan(beacon.angle[3]) - tan(beacon.angle[2]));
			}
			opponent.x = tan(beacon.angle[2]) * opponent.y;
			break;
		default:
// 			printf("[position.c] => Unknown Formula\r\n");
			break;
	}
}