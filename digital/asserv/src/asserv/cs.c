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

encoder_t encoder_left, encoder_right;
encoder_corrector_t encoder_right_corrector;
output_t output_left, output_right;
control_system_polar_t cs_main;

#if AC_ASSERV_AUX_NB
encoder_t encoder_aux[AC_ASSERV_AUX_NB];
output_t output_aux[AC_ASSERV_AUX_NB];
control_system_single_t cs_aux[AC_ASSERV_AUX_NB];
#endif

void
cs_init (void)
{
    /* Initialise encoders. */
    encoder_init (0, &encoder_left);
    encoder_init (1, &encoder_right);
    encoder_corrector_init (&encoder_right_corrector);
    /* Initialise outputs. */
    output_left.max = OUTPUT_MAX;
    output_init (0, &output_left);
    output_right.max = OUTPUT_MAX;
    output_init (1, &output_right);
    /* Initialise control system. */
    control_system_polar_init (&cs_main);
    cs_main.encoder_left = &encoder_left;
    cs_main.encoder_right = &encoder_right;
    cs_main.output_left = &output_left;
    cs_main.output_right = &output_right;
#if AC_ASSERV_AUX_NB
    uint8_t i;
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
      {
	/* Initialise encoders. */
	encoder_init (2 + i, &encoder_aux[i]);
	/* Initialise outputs. */
	output_aux[i].max = OUTPUT_MAX;
	output_init (2 + i, &output_aux[i]);
	/* Initialise control system. */
	control_system_single_init (&cs_aux[i]);
	cs_aux[i].encoder = &encoder_aux[i];
	cs_aux[i].output = &output_aux[i];
      }
#endif
}

void
cs_update_prepare (void)
{
    control_system_polar_update_prepare (&cs_main);
#if AC_ASSERV_AUX_NB
    uint8_t i;
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
	control_system_single_update_prepare (&cs_aux[i]);
#endif
}

void
cs_update (void)
{
    control_system_polar_update (&cs_main);
#if AC_ASSERV_AUX_NB
    uint8_t i;
    for (i = 0; i < AC_ASSERV_AUX_NB; i++)
	control_system_single_update (&cs_aux[i]);
#endif
}

