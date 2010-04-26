#ifndef vect_h
#define vect_h
/* vect.h */
/* avr.math.geometry - Geometry math module. {{{
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

/* See fixed.h for numbers notation convention.  Angles are in the [0, 1)
 * interval, counter clock wise, uf0.16 unless noted. */

/** Vector single value.  As we are working with millimeters most of the time,
 * 16 bits are enough.  There is no magic, if you change this, code should be
 * reviewed. */
typedef int16_t vect_value_t;

/** 2D vector. */
struct vect_t
{
    /** X coordinate. */
    vect_value_t x;
    /** Y coordinate. */
    vect_value_t y;
};
typedef struct vect_t vect_t;

/** Scale (multiply) a vector by a fixed point value.
 * - v: input/result vector.
 * - s_f824: scale (f8.24). */
void
vect_scale_f824 (vect_t *v, int32_t s_f824);

/** Rotate a vector.
 * - v: input/result vector.
 * - a_uf016: angle of rotation (uf0.16). */
void
vect_rotate_uf016 (vect_t *v, uint16_t a_uf016);

/** Translate a vector (add two vectors).
 * - v: input/result vector.
 * - t: translation vector. */
void
vect_translate (vect_t *v, const vect_t *t);

/** Same as vect_translate. */
#define vect_add vect_translate

/** Substract two vectors.
 * - v: input/result vector.
 * - n: substracted vector. */
void
vect_sub (vect_t *v, const vect_t *n);

/** Compute vector norm (length).
 * - v: input vector.
 * - returns: norm. */
vect_value_t
vect_norm (const vect_t *v);

/** Compute a vector from polar coordinates.
 * - v: result vector.
 * - l: vector length.
 * - a_uf016: vector angle (uf0.16). */
void
vect_from_polar_uf016 (vect_t *v, int16_t l, uint16_t a_uf016);

/** Compute a vector normal (rotate the vector counter-clockwise of pi/2).
 * - v: input/result vector. */
void
vect_normal (vect_t *v);

/** Compute the dot product (scalar product) of two vectors.
 * - a, b: vectors to make product of.
 * - returns: dot product. */
int32_t
vect_dot_product (const vect_t *a, const vect_t *b);

/** Compute the dot product (scalar product) of a normal of the first vector
 * with the second vector.
 * - a: vector to take normal of.
 * - b: vector to make product with.
 * - returns: dot product.
 * The first vector is rotated of pi/2.  The result is the same as the z part
 * of the vector product. */
int32_t
vect_normal_dot_product (const vect_t *a, const vect_t *b);

/** Scale (multiply) vectors by a fixed point value.
 * - va: input/result vectors array.
 * - vn: number of vectors.
 * - s_f824: scale (f8.24). */
void
vect_array_scale_f824 (vect_t *va, uint8_t vn, int32_t s_f824);

/** Rotate vectors.
 * - va: input/result vectors array.
 * - vn: number of vectors.
 * - a_uf016: angle of rotation (uf0.16). */
void
vect_array_rotate_uf016 (vect_t *va, uint8_t vn, uint16_t a_uf016);

/** Translate vectors.
 * - va: input/result vectors array.
 * - vn: number of vectors.
 * - t: translation vector. */
void
vect_array_translate (vect_t *va, uint8_t vn, const vect_t *t);

#endif /* vect_h */
