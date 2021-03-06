/* cs.c - Control system definition. */
/*  {{{
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
#include "cs.h"

encoder_t encoder_aux[AC_ASSERV_AUX_NB];
output_t output_aux[AC_ASSERV_AUX_NB];

control_system_single_t cs_aux[AC_ASSERV_AUX_NB];

void
cs_init (void)
{
    uint8_t i;
    /* Initialise encoders. */
    encoder_init (0, &encoder_aux[0]);
    encoder_init (1, &encoder_aux[1]);
    /* Initialise outputs. */
    output_aux[0].max = OUTPUT_MAX;
    output_init (0, &output_aux[0]);
    output_aux[1].max = OUTPUT_MAX;
    output_init (1, &output_aux[1]);
    /* Initialise control system. */
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
      {
	control_system_single_init (&cs_aux[i]);
	cs_aux[i].encoder = &encoder_aux[i];
	cs_aux[i].output = &output_aux[i];
      }
}

void
cs_update_prepare (void)
{
    uint8_t i;
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
	control_system_single_update_prepare (&cs_aux[i]);
}

void
cs_update (void)
{
    uint8_t i;
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
	control_system_single_update (&cs_aux[i]);
}

void
cs_reset (void)
{
    uint8_t i;
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
      {
	output_set (&output_aux[i], 0);
	output_brake (&output_aux[i], 0);
	control_state_set_mode (&cs_aux[i].state, CS_MODE_NONE, 0);
      }
    output_update ();
}

