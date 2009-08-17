/* test_path.c */
/* avr.path - Path finding module. {{{
 *
 * Copyright (C) 2008 Nicolas Schodet
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
#include "common.h"
#include "modules/path/path.h"

#include <stdio.h>
#include <stdlib.h>

void
syntax (void)
{
    fprintf (stderr,
	     "test_path borders source destination escape_factor"
	     " [obstacles (0-%d)]\n"
	     "  borders: xmin,ymin,xmax,ymax\n"
	     "  source, destination: x,y\n"
	     "  obstacles: x,y,r,f\n"
	     "example: test_path 0,0,1500,1500 300,750 1200,750 0"
	     " 600,680,100,0"
	     " 900,820,100,10\n",
	     AC_PATH_OBSTACLES_NB);
    exit (1);
}

/** Read a comma separated list of N integers into TAB from S.  Exit on
 * error. */
void
read_tab (const char *s, int *tab, int n)
{
    assert (n > 0);
    while (n)
      {
	char *sp;
	int v;
	v = strtol (s, &sp, 10);
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

int
main (int argc, char **argv)
{
    if (argc < 5 || argc > 5 + AC_PATH_OBSTACLES_NB)
	syntax ();
    int tab[4];
    read_tab (argv[1], tab, 4);
    path_init (tab[0], tab[1], tab[2], tab[3]);
    read_tab (argv[2], tab, 2);
    read_tab (argv[3], tab + 2, 2);
    path_endpoints (tab[0], tab[1], tab[2], tab[3]);
    read_tab (argv[4], tab, 1);
    if (tab[0])
	path_escape (tab[0]);
    int i;
    for (i = 0; i + 5 < argc; i++)
      {
	read_tab (argv[5 + i], tab, 4);
	path_obstacle (i, tab[0], tab[1], tab[2], tab[3], 1);
      }
    path_update ();
    path_print_graph ();
    uint16_t x, y;
    if (path_get_next (&x, &y))
	printf ("// Next point: %d, %d\n", x, y);
    else
	printf ("// Failure\n");
    return 0;
}

void
path_report (uint16_t *points, uint8_t len,
	     struct path_obstacle_t *obstacles, uint8_t obstacles_nb)
{
}

