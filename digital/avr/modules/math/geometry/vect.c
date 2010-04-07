/* vect.c */
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
#include "common.h"
#include "vect.h"

#include "modules/math/fixed/fixed.h"

void
vect_scale_f824 (vect_t *v, int32_t s_f824)
{
    assert (v);
    v->x = fixed_mul_f824 (v->x, s_f824);
    v->y = fixed_mul_f824 (v->y, s_f824);
}

void
vect_rotate_uf016 (vect_t *v, uint16_t a_uf016)
{
    assert (v);
    int32_t c, s;
    uint32_t a_uf824 = ((uint32_t) a_uf016) << 8;
    c = fixed_cos_f824 (a_uf824);
    s = fixed_sin_f824 (a_uf824);
    int16_t tx = v->x, ty = v->y;
    v->x = fixed_mul_f824 (tx, c) - fixed_mul_f824 (ty, s);
    v->y = fixed_mul_f824 (tx, s) + fixed_mul_f824 (ty, c);
}

void
vect_translate (vect_t *v, const vect_t *t)
{
    assert (v);
    assert (t);
    v->x = v->x + t->x;
    v->y = v->y + t->y;
}

void
vect_sub (vect_t *v, const vect_t *n)
{
    assert (v);
    assert (n);
    v->x = v->x - n->x;
    v->y = v->y - n->y;
}

vect_value_t
vect_norm (const vect_t *v)
{
    assert (v);
    int16_t tx = v->x, ty = v->y;
    return fixed_sqrt_ui32 (tx * tx + ty * ty);
}

void
vect_from_polar_uf016 (vect_t *v, int16_t l, uint16_t a_uf016)
{
    assert (v);
    uint32_t a_uf824 = ((uint32_t) a_uf016) << 8;
    v->x = fixed_mul_f824 (l, fixed_cos_f824 (a_uf824));
    v->y = fixed_mul_f824 (l, fixed_sin_f824 (a_uf824));
}

void
vect_array_scale_f824 (vect_t *va, uint8_t vn, int32_t s_f824)
{
    for (; vn; va++, vn--)
	vect_scale_f824 (va, s_f824);
}

void
vect_array_rotate_uf016 (vect_t *va, uint8_t vn, uint16_t a_uf016)
{
    assert (va);
    int32_t c, s;
    uint32_t a_uf824 = ((uint32_t) a_uf016) << 8;
    c = fixed_cos_f824 (a_uf824);
    s = fixed_sin_f824 (a_uf824);
    for (; vn; va++, vn--)
      {
	int16_t tx = va->x, ty = va->y;
	va->x = fixed_mul_f824 (tx, c) - fixed_mul_f824 (ty, s);
	va->y = fixed_mul_f824 (tx, s) + fixed_mul_f824 (ty, c);
      }
}

void
vect_array_translate (vect_t *va, uint8_t vn, const vect_t *t)
{
    for (; vn; va++, vn--)
	vect_translate (va, t);
}
