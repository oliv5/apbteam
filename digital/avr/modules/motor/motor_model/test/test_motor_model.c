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
#include "modules/motor/motor_model/motor_model_defs.host.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
simu (motor_model_t *m, double t)
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
    double u_max, load;
    motor_model_t ms, *m;
    const motor_model_def_t *md;
    /* Check arguments. */
    if (argc != 4)
      {
	fprintf (stderr, "syntax: %s MODEL UMAX LOAD\n", argv[0]);
	return 1;
      }
    u_max = atof (argv[2]);
    load = atof (argv[3]);
    md = motor_model_def_get (argv[1]);
    if (!md)
      {
	fprintf (stderr, "model unknown\n");
	return 1;
      }
    m = &ms;
    memset (m, 0, sizeof (*m));
    m->m = *md;
    m->m.J = load;
    m->h = 1e-3;
    m->d = 1000;
    /* Make a step response simulation. */
    printf ("# %10s %12s %12s %12s %12s\n", "t", "u", "i", "omega", "theta");
    m->u = u_max;
    printf ("%12f %12f %12f %12f %12f\n", m->t, m->u, m->i, m->o, m->th);
    simu (m, 1.0);
    m->u = 0.0;
    simu (m, 1.0);
    return 0;
}

