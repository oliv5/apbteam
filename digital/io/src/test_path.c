/* test_path.c */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
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
#include "path.h"

#include <stdio.h>

void
path_print_graph (void);

int
main (void)
{
    path_init (0, 0, 3000, 2100);
    path_endpoints (300, 1000, 1200, 1000);
    path_obstacle (0, 600, 930, 100, 1);
    path_obstacle (1, 900, 1070, 100, 1);
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
simu_send_path (uint8_t len, uint16_t *points)
{
}

