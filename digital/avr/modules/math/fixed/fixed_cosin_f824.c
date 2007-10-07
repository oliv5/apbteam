/* fixed_cosin_f824.c */
/* avr.math.fixed - Fixed point math module. {{{
 *
 * Copyright (C) 2005 Nicolas Schodet
 *
 * Robot APB Team/Efrei 2006.
 *        Web: http://assos.efrei.fr/robot/
 *      Email: robot AT efrei DOT fr
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
#include "fixed.h"

/** Compute cosinus for angles between [0,pi/2]. */
static int32_t
fixed_cos_dli (int32_t a)
{
    static const int32_t f[] = {
	(1L << 24) * -26.42625678337439745096,
	(1L << 24) * 60.24464137187666035919,
	(1L << 24) * -85.45681720669372773226,
	(1L << 24) * 64.93939402266829148905,
	(1L << 24) * -19.73920880217871723738,
	(1L << 24) * 1
    };
    int32_t r;
    int32_t a2;
    uint8_t i;
    a2 = fixed_mul_f824 (a, a);
    r = f[0];
    for (i = 1; i < sizeof (f) / sizeof (f[0]); i++)
	r = fixed_mul_f824 (r, a2) + f[i];
    return r;
}

/** Compute cosinus, angle f8.24, result f8.24. */
int32_t
fixed_cos_f824 (int32_t a)
{
    a &= (1L << 24) - 1;
    uint8_t z = ((uint32_t) a >> 16) & 0xc0;
    if (z == 0)
	return fixed_cos_dli (a);
    else if (z == 1 << 6)
	return -fixed_cos_dli ((1L << 23) - a);
    else if (z == 2 << 6)
	return -fixed_cos_dli (a & 0xff7fffff);
    else
	return fixed_cos_dli ((1L << 24) - a);
}

/** Compute sinus, angle f8.24, result f8.24. */
int32_t
fixed_sin_f824 (int32_t a)
{
    a &= (1L << 24) - 1;
    uint8_t z = ((uint32_t) a >> 16) & 0xc0;
    if (z == 0)
	return fixed_cos_dli ((1L << 22) - a);
    else if (z == 1 << 6)
	return fixed_cos_dli (a - (1L << 22));
    else if (z == 2 << 6)
	return -fixed_cos_dli ((1L << 23) + (1L << 22) - a);
    else
	return -fixed_cos_dli (a - (1L << 23) - (1L << 22));
}

