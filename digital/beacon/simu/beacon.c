/* beacon.c */
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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "position.h"
#include "common.h"

/* Globals Declaration */
beacon_value_s beacon;
opponent_s opponent;

void syntax (void)
{
    fprintf (stderr,
	     "beacon sources_number source1 angle1 ... sourceX angleX\n"
	     "example: beacon 2,1,3.2,3,0.82\n"
	     );
    exit (1);
}

void read_tab (const char *s, double *tab, int n)
{
	assert (n > 0);
	while (n)
	{
		char *sp;
		double v;
		v = strtod (s, &sp);
		if (sp == s)
		syntax ();
		if ((n > 1 && *sp != ',')
		|| (n == 1 && *sp != '\0'))
		syntax ();
		s = sp + 1;
		*tab++ = v;
		n--;
	}
}

int main (int argc, char **argv)
{
	if (argc < 8 || argc > 10)
		syntax ();
	
	double input[7] = {0};
	/* Init global structures */
	init_struct();
	
	
	/* Read Beacon num */
	read_tab (argv[2], input, 1);
	
	/* Read Beacon angle */
	read_tab (argv[3], input+1, 1);
	
	/* Read Beacon num */
	read_tab (argv[4], input+2, 1);
	
	/* Read Beacon angle */
	read_tab (argv[5], input+3, 1);
	
	/* Read Activate filter */
	read_tab (argv[6],input+4, 1);
	
	/* Read last x */
	read_tab (argv[7],input+5, 1);

	/* Read last y */
	read_tab (argv[8],input+6, 1);	
	
// 	printf("Balise %d --- Angle %f\n",(int)input[0],input[1]);
// 	printf("Balise %d --- Angle %f\n",(int)input[2],input[3]);

	/* Compute position */
  	update_position(input[0],input[1]);
  	update_position(input[2],input[3]);
	
	/* Return position to the simulator */
	printf("%d\n",(int)opponent.x);
	printf("%d\n",(int)opponent.y);
	
	return 0;
}
