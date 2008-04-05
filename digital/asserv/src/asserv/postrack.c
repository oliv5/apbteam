/* postrack.c - Compute current position from counters. */
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
#include "postrack.h"

#include "modules/math/fixed/fixed.h"
#include "modules/math/math.h"

#include "counter.h"

/** Current position, f24.8. */
int32_t postrack_x, postrack_y;
/** Current angle, f8.24. */
int32_t postrack_a;
/** Distance between the weels, u16. */
uint16_t postrack_footing;
/** Precomputed footing factor, f8.24.
 * postrack_footing_factor = (1/2pi * 256) / postrack_footing
 * Explanations:
 *  - Angles are between 0 and 1, corresponding to 0 and 2pi, therefore we
 *  must divide by 2pi to convert unit (Arc=Angle * Radius only works for
 *  radians).
 *  - dd (see postrack_update) is in f10.16 format, we multiply by 256 to have
 *  a angle in f8.24 format.
 *  - this factor is in f8.24 format, therefore, 1 is writen (1L << 24). */
static uint32_t postrack_footing_factor;

/** Initialise the position tracker. */
void
postrack_init (void)
{
    /* Prevent division by 0 by providing a default large value. */
    postrack_set_footing (0x1000);
}

/** Update the current position. */
void
postrack_update (void)
{
    int32_t d, dd, da, na, dsc;
    d = counter_right_diff + counter_left_diff;		/* 10b */
    d <<= 16;						/* 10.16b */
    if (counter_right_diff == counter_left_diff)
      {
	/* Line. */
	postrack_x += fixed_mul_f824 (d, fixed_cos_f824 (postrack_a)) >> 8;
	postrack_y += fixed_mul_f824 (d, fixed_sin_f824 (postrack_a)) >> 8;
      }
    else
      {
	/* Arc. */
	dd = counter_right_diff - counter_left_diff;	/* 10b */
	dd <<= 16;					/* 10.16b */
	da = fixed_mul_f824 (dd, postrack_footing_factor);/* 8.24b */
	/* New angle. */
	na = postrack_a + da;
	/* Compute da in radians. */
	da = fixed_mul_f824 (da, 2 * M_PI * (1L << 24));
	/* X increment. */
	dsc = fixed_sin_f824 (na) - fixed_sin_f824 (postrack_a); /* 8.24b */
	dsc = fixed_div_f824 (dsc, da);			/* 8.24b < 1 */
	postrack_x += fixed_mul_f824 (d, dsc) >> 8;	/* 24.8b */
	/* Y increment. */
	dsc = fixed_cos_f824 (postrack_a) - fixed_cos_f824 (na);	/* 8.24b */
	dsc = fixed_div_f824 (dsc, da);			/* 8.24b < 1 */
	postrack_y += fixed_mul_f824 (d, dsc) >> 8;	/* 24.8b */
	/* Angle update. */
	postrack_a = na;
	postrack_a &= 0x00ffffff;
      }
}

/** Change the footing value. */
void
postrack_set_footing (uint16_t footing)
{
    postrack_footing = footing;
    postrack_footing_factor =
	(uint32_t) (0.5 * M_1_PI * (1L << 8) * (1L << 24)) / footing;
}

