/* motor_model_defs.host.c */
/* motor - Motor control module. {{{
 *
 * Copyright (C) 2011 Nicolas Schodet
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
#include "motor_model_defs.host.h"

#include <math.h>
#include <string.h>

/* RE25CLL with 1:10 gearbox. */
const motor_model_def_t motor_model_def_re25cll_x10 =
{
    /* Motor characteristics. */
    407 * (2*M_PI) / 60,/* Speed constant ((rad/s)/V). */
    23.4 / 1000,	/* Torque constant (N.m/A). */
    0,			/* Bearing friction (N.m/(rad/s)). */
    2.18,		/* Terminal resistance (Ohm). */
    0.24 / 1000,	/* Terminal inductance (H). */
    /* Gearbox characteristics. */
    10,			/* Gearbox ratio. */
    0.75,		/* Gearbox efficiency. */
    /* Load characteristics. */
    0.0,		/* Load (kg.m^2). */
    /* Hardware limits. */
    -INFINITY, +INFINITY,
};

/* RE25G with 1:20.25 gearbox. */
const motor_model_def_t motor_model_def_re25g_x20_25 =
{
    /* Motor characteristics. */
    407 * (2*M_PI) / 60,/* Speed constant ((rad/s)/V). */
    23.4 / 1000,	/* Torque constant (N.m/A). */
    0,			/* Bearing friction (N.m/(rad/s)). */
    2.32,		/* Terminal resistance (Ohm). */
    0.24 / 1000,	/* Terminal inductance (H). */
    /* Gearbox characteristics. */
    20.25,		/* Gearbox ratio. */
    0.75,		/* Gearbox efficiency. */
    /* Load characteristics. */
    0.0,		/* Load (kg.m^2). */
    /* Hardware limits. */
    -INFINITY, +INFINITY,
};

/* AMAX32GHP with 1:16 gearbox. */
const motor_model_def_t motor_model_def_amax32ghp_x16 =
{
    /* Motor characteristics. */
    269 * (2*M_PI) / 60,/* Speed constant ((rad/s)/V). */
    25.44 / 1000,	/* Torque constant (N.m/A). */
    0,			/* Bearing friction (N.m/(rad/s)). */
    3.99,		/* Terminal resistance (Ohm). */
    0.24 / 1000,	/* Terminal inductance (H). */
    /* Gearbox characteristics. */
    16,			/* Gearbox ratio. */
    0.75,		/* Gearbox efficiency. */
    /* Load characteristics. */
    0.0,		/* Load (kg.m^2). */
    /* Hardware limits. */
    -INFINITY, +INFINITY,
};

/* Faulhaber 2657 with 1:9.7 gearbox. */
const motor_model_def_t motor_model_def_faulhaber_2657_x9_7 =
{
    /* Motor characteristics. */
    274 * (2*M_PI) / 60,/* Speed constant ((rad/s)/V). */
    34.8 / 1000,	/* Torque constant (N.m/A). */
    0,			/* Bearing friction (N.m/(rad/s)). */
    2.84,		/* Terminal resistance (Ohm). */
    0.380 / 1000,	/* Terminal inductance (H). */
    /* Gearbox characteristics. */
    9.7,		/* Gearbox ratio. */
    0.80,		/* Gearbox efficiency. */
    /* Load characteristics. */
    0.0,		/* Load (kg.m^2). */
    /* Hardware limits. */
    -INFINITY, +INFINITY,
};

/* Faulhaber 2342 with 23/1 1:3.71 gearbox. */
const motor_model_def_t motor_model_def_faulhaber_2342_x3_71 =
{
    /* Motor characteristics. */
    366 * (2*M_PI) / 60,/* Speed constant ((rad/s)/V). */
    26.10 / 1000,	/* Torque constant (N.m/A). */
    0,			/* Bearing friction (N.m/(rad/s)). */
    7.10,		/* Terminal resistance (Ohm). */
    0.265 / 1000,	/* Terminal inductance (H). */
    /* Gearbox characteristics. */
    3.71,		/* Gearbox ratio. */
    0.88,		/* Gearbox efficiency. */
    /* Load characteristics. */
    0.0,		/* Load (kg.m^2). */
    /* Hardware limits. */
    -INFINITY, +INFINITY,
};

struct motor_model_def_table_t
{
    const char *name;
    const motor_model_def_t *def;
};

static const struct motor_model_def_table_t def_table[] =
{
      { "re25cll_x10", &motor_model_def_re25cll_x10 },
      { "re25g_x20_25", &motor_model_def_re25g_x20_25 },
      { "amax32ghp_x16", &motor_model_def_amax32ghp_x16 },
      { "faulhaber_2657_x9_7", &motor_model_def_faulhaber_2657_x9_7 },
      { "faulhaber_2342_x3_71", &motor_model_def_faulhaber_2342_x3_71 },
      { NULL, NULL }
};

const motor_model_def_t *
motor_model_def_get (const char *name)
{
    const struct motor_model_def_table_t *p = def_table;
    for (p = def_table; p->name; p++)
      {
	if (strcmp (p->name, name) == 0)
	    return p->def;
      }
    return NULL;
}

