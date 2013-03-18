#ifndef fixed_h
#define fixed_h
/* fixed.h */
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

/* Numbers notation:
 * [u]{i|f}x[.y]
 * u: unsigned
 * i: integer
 * f: fixed point
 * x: integral part size in bits
 * y: fractionnal part size in bits
 *
 * Ex: i16: signed 16 bit word, uf8.8: unsigned fixed 8.8.
 *
 * Angles are mapped from [0, 2pi) to [0,1). */

#ifdef TARGET_avr

/* Multiply f8.24 by f8.24, return f8.24. */
#define fixed_mul_f824(a, b) ({ \
    uint16_t dummy; \
    asm ("" : "=r" (dummy)); \
    fixed_mul_f824_asm (dummy, a, b); })
int32_t
fixed_mul_f824_asm (uint16_t dummy, int32_t a, int32_t b);

/* Divide f8.24 by f8.24, return f8.24. */
int32_t
fixed_div_f824 (int32_t a, int32_t b);

#else /* !TARGET_avr */

/** Multiply f8.24 by f8.24, return f8.24. */
extern inline
int32_t
fixed_mul_f824 (int32_t a, int32_t b)
{
    return ((int64_t) a * b + 0x800000) >> 24;
}

/** Divide f8.24 by f8.24, return f8.24. */
extern inline
int32_t
fixed_div_f824 (int32_t a, int32_t b)
{
    if ((a ^ b) >= 0)
	return (((int64_t) a << 24) + b / 2) / b;
    else
	return (((int64_t) a << 24) - b / 2) / b;
}

#endif

/** Compute cosinus, angle f8.24, result f8.24. */
int32_t
fixed_cos_f824 (int32_t a);

/** Compute sinus, angle f8.24, result f8.24. */
int32_t
fixed_sin_f824 (int32_t a);

/** Compute square root, uf24.8. */
uint32_t
fixed_sqrt_uf248 (uint32_t x);

/** Compute square root, ui32 -> ui16. */
uint16_t
fixed_sqrt_ui32 (uint32_t x);

#endif /* fixed_h */
