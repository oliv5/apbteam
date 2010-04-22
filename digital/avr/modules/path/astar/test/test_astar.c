/* test_astar.c */
/* avr.path.astar - A* path finding module. {{{
 *
 * Copyright (C) 2010 Nicolas Schodet
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
#include "modules/path/astar/astar.h"
#include "modules/utils/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASTAR_DEBUG 0

void
syntax (void)
{
    fprintf (stderr,
	     "test_astar map...\n"
	     "  map: map lines, use `_' for free nodes, `X' for blocked"
	     " nodes, `i' for\n"
	     "initial node and `g' for goal node.\n"
	     "example: test_astar iX___\n"
	     "                    _X_X_\n"
	     "                    ___Xg\n"
	     "This test program will use a rectangular grid but the astar"
	     " module can be\n"
	     "used with many other graph.\n");
    exit (1);
}

/** Cost of an orthogonal move. */
#define TEST_WEIGHT_ORTH 100

/** Cost of a diagonal move. */
#define TEST_WEIGHT_DIAG 141

/** Global as there is no context. */
int test_width, test_height, test_initial_x, test_initial_y;

/** Global map, see syntax. */
char *test_map;

int
main (int argc, const char **argv)
{
    int x, y, i;
    const char **maps, *p;
    int initial = -1, goal = -1;
    if (argc < 2)
	syntax ();
    test_width = strlen (argv[1]);
    test_height = argc - 1;
    char map[test_width * test_height + 1];
    test_map = map;
    /* Read map. */
    for (y = 0, i = 0, maps = &argv[1];
	 y < test_height;
	 y++, maps++)
	for (x = 0, p = *maps;
	     x < test_width;
	     x++, i++)
	  {
	    map[i] = *p;
	    switch (*p++)
	      {
	      case 'g':
		goal = y * test_width + x;
		break;
	      case 'i':
		initial = y * test_width + x;
		test_initial_x = x;
		test_initial_y = y;
		break;
	      case '_':
	      case 'X':
		break;
	      default:
		syntax ();
	      }
	  }
    map[i] = '\0';
    if (initial == -1 || goal == -1)
	syntax ();
    /* Compute path, invert initial and goal so that path can be easily
     * extracted. */
    struct astar_node_t nodes[test_width * test_height];
    if (astar (nodes, test_width * test_height, goal, initial))
      {
	int n = initial, nn = -1, ord = 0;
	while (nn != goal)
	  {
	    int x = n % test_width,
		y = n / test_width;
	    printf ("%d, %d\n", x, y);
	    map[n] = '0' + ord++ % 10;
	    nn = n;
	    n = nodes[n].prev;
	  }
	printf ("\n");
	for (y = 0; y < test_height; y++)
	    printf ("// %.*s\n", test_width, map + y * test_width);
      }
    else
      {
	printf ("failure\n");
      }
    return 0;
}

uint8_t
test_neighbor_callback (uint8_t node, struct astar_neighbor_t *neighbors)
{
    static const struct {
	int dx, dy, weight;
    } n[] = {
	  { -1, -1, TEST_WEIGHT_DIAG }, /* NW */
	  { 0, -1, TEST_WEIGHT_ORTH }, /* N */
	  { 1, -1, TEST_WEIGHT_DIAG }, /* NE */
	  { -1, 0, TEST_WEIGHT_ORTH }, /* W */
	  { 1, 0, TEST_WEIGHT_ORTH }, /* E */
	  { -1, 1, TEST_WEIGHT_DIAG }, /* SW */
	  { 0, 1, TEST_WEIGHT_ORTH }, /* S */
	  { 1, 1, TEST_WEIGHT_DIAG }, /* SE */
    };
    int x = node % test_width,
	y = node / test_width;
    if (ASTAR_DEBUG)
	printf ("neighbors %d, %d\n", x, y);
    int neighbors_nb = 0;
    int i;
    for (i = 0; i < (int) UTILS_COUNT (n); i++)
      {
	int nx = x + n[i].dx,
	    ny = y + n[i].dy;
	if (nx < 0 || nx >= test_width || ny < 0 || ny >= test_height)
	    continue;
	int node = ny * test_width + nx;
	if (test_map[node] == 'X')
	    continue;
	neighbors[neighbors_nb].node = node;
	neighbors[neighbors_nb].weight = n[i].weight;
	neighbors_nb++;
      }
    return neighbors_nb;
}

uint16_t
test_heuristic_callback (uint8_t node)
{
    /* Diagonal move are allowed, but not any angle. */
    int x = node % test_width,
	y = node / test_width;
    int dx = UTILS_ABS (test_initial_x - x),
	dy = UTILS_ABS (test_initial_y - y);
    int min = UTILS_MIN (dx, dy),
	max = UTILS_MAX (dx, dy);
    int h = (max - min) * TEST_WEIGHT_ORTH + min * TEST_WEIGHT_DIAG;
    if (ASTAR_DEBUG)
	printf ("heuristic %d, %d, %d\n", x, y, h);
    return h;
}

