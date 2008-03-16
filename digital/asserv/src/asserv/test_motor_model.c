/* test_motor_model.c - Test DC motor model. */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2006 Nicolas Schodet
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
#include "motor_model.host.h"
#include "models.host.h"

#include <stdio.h>

void simu (struct motor_t *m, double t)
{
    int i, s;
    s = t / m->h;
    for (i = 0; i < s; i++)
      {
	motor_model_step (m);
	printf ("%12f %12f %12f %12f %12f\n", m->t, m->u, m->i, m->o, m->th);
      }
}

int
main (int argc, char **argv)
{
    struct motor_t ms;
    struct motor_t *m;
    const struct robot_t *mr;
    /* Check arguments. */
    if (argc != 2)
      {
	fprintf (stderr, "syntax: %s MODEL\n", argv[0]);
	return 1;
      }
    mr = models_get (argv[1]);
    if (!mr)
      {
	fprintf (stderr, "model unknown\n");
	return 1;
      }
    models_init (mr, &ms, NULL, NULL);
    m = &ms;
    /* Make a step response simulation. */
    printf ("# %10s %12s %12s %12s %12s\n", "t", "u", "i", "omega", "theta");
    m->u = m->m.u_max;
    printf ("%12f %12f %12f %12f %12f\n", m->t, m->u, m->i, m->o, m->th);
    simu (m, 1.0);
    m->u = 0.0;
    simu (m, 1.0);
    return 0;
}
