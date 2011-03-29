/* encoder_corrector.c */
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
#include "encoder_corrector.h"

#include "modules/math/fixed/fixed.h"

/*
 * The encoder corrector apply a correction factor to an encoder output to
 * take into account a difference in wheel size.  This should be used on the
 * right encoder for example for a two wheeled robot.
 */

void
encoder_corrector_init (encoder_corrector_t *corrector)
{
    /* Default value: no correction. */
    corrector->correction = 1L << 24;
}

void
encoder_corrector_update (encoder_corrector_t *corrector, encoder_t *encoder)
{
    /* First cancel encoder update work. */
    encoder->cur -= encoder->diff;
    /* Update our own raw encoder position and apply factor. */
    corrector->cur_raw += encoder->diff;
    uint16_t new = fixed_mul_f824 (corrector->cur_raw, corrector->correction);
    /* Now patch encoder state. */
    encoder->diff = (int16_t) (new - encoder->cur);
    encoder->cur = new;
}

