/* main_simu.c */
/* Beacon Simulator Interface. {{{
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
#ifdef SIMULATOR

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "position.h"

/* Globals Declaration */
extern opponent_s opponent[MAX_OBSTACLE];

int main (int argc, char **argv)
{
	char saisie[30];
	char ret = 0;
	int id = 0;
	float angle = 0;
	int angle_id = 0;
	int i = 0;
	/* Init global structures */
  	position_init_struct();

	while(1)
	{
		ret = fgets (saisie, sizeof saisie, stdin);
		if(ret != NULL)
		{
			id = strtol (saisie, NULL, 10);
		}
		ret = fgets (saisie, sizeof saisie, stdin);
		if(ret != NULL)
		{
			angle = strtod (saisie, NULL);
		}
		ret = fgets (saisie, sizeof saisie, stdin);
		if(ret != NULL)
		{
			angle_id = strtod (saisie, NULL);
		}
		
  		update_position(id,angle_id,angle);
		 
		/* Return position to the simulator */
		for(i=1;i<=MAX_OBSTACLE;i++)
		{
			printf("%d\n",(int)opponent[i].x);
			printf("%d\n",(int)opponent[i].y);
			printf("%d\n",(int)opponent[i].trust);
		}
		fflush(stdout);
	}
	return 0;
}
#endif //SIMULATOR
