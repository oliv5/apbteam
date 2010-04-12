/* test_geometry.c */
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
#include "modules/math/geometry/geometry.h"
#include "modules/math/geometry/vect.h"

#include "modules/utils/utils.h"
#include "modules/uart/uart.h"

#ifdef HOST
#define test assert
#else
#define test(p) \
    do { \
	const char *pp; \
	for (pp = #p; *pp; pp++) \
	    uart0_putc (*pp); \
	uart0_putc ('\n'); \
	if (!(p)) while (1); \
    } while (0);
#endif

void
test_geometry (void)
{
    test (G_ANGLE_UF016_DEG (0) == 0);
    test (G_ANGLE_UF016_DEG (45) == 0x2000);
    test (G_ANGLE_UF016_DEG (90) == 0x4000);
    test (G_ANGLE_UF016_DEG (135) == 0x6000);
    test (G_ANGLE_UF016_DEG (180) == 0x8000);
    test (G_ANGLE_UF016_DEG (225) == 0xa000);
    test (G_ANGLE_UF016_DEG (270) == 0xc000);
    test (G_ANGLE_UF016_DEG (315) == 0xe000);
    test (G_ANGLE_UF016_DEG (360) == 0x0000);
    test (G_ANGLE_UF016_DEG (-45) == 0xe000);
    test (G_ANGLE_UF016_DEG (-90) == 0xc000);
    test (G_ANGLE_UF016_DEG (-135) == 0xa000);
    test (G_ANGLE_UF016_DEG (-180) == 0x8000);
    test (G_ANGLE_UF016_DEG (-225) == 0x6000);
    test (G_ANGLE_UF016_DEG (-270) == 0x4000);
    test (G_ANGLE_UF016_DEG (-315) == 0x2000);
    test (G_ANGLE_UF016_DEG (-360) == 0x0000);
    test (G_ANGLE_UF016_RAD (0) == 0);
    test (G_ANGLE_UF016_RAD (M_PI / 4) == 0x2000);
    test (G_ANGLE_UF016_RAD (M_PI / 2) == 0x4000);
    test (G_ANGLE_UF016_RAD (M_PI) == 0x8000);
    test (G_ANGLE_UF016_RAD (3 * M_PI / 2) == 0xc000);
    test (G_ANGLE_UF016_RAD (2 * M_PI) == 0x0000);
    test (G_ANGLE_UF016_RAD (-M_PI / 4) == 0xe000);
    test (G_ANGLE_UF016_RAD (-M_PI / 2) == 0xc000);
    test (G_ANGLE_UF016_RAD (-M_PI) == 0x8000);
    test (G_ANGLE_UF016_RAD (-3 * M_PI / 2) == 0x4000);
    test (G_ANGLE_UF016_RAD (-2 * M_PI) == 0x0000);
}

void
test_vect (void)
{
    int i;
    int16_t n;
    int32_t p;
    vect_t a, b;
    /* vect_*. */
    a.x = 50; a.y = 0;
    b.x = 500; b.y = 500;
    vect_scale_f824 (&a, 0x2000000);
    vect_scale_f824 (&b, -0x2000000);
    test (a.x == 100 && a.y == 0);
    test (b.x == -1000 && b.y == -1000);
    for (i = 0; i < 6; i++)
	vect_rotate_uf016 (&a, G_ANGLE_UF016_DEG (30));
    /* There is inevitable rounding errors. */
    test (a.x >= -101 && a.x <= -99 && a.y >= -1 && a.y <= 1);
    for (i = 0; i < 4; i++)
	vect_rotate_uf016 (&b, G_ANGLE_UF016_DEG (45));
    test (b.x == 1000 && b.y == 1000);
    a.x = 2; a.y = 4;
    b.x = 8; b.y = 16;
    vect_translate (&a, &b);
    test (a.x == 10 && a.y == 20);
    vect_sub (&a, &b);
    test (a.x == 2 && a.y == 4);
    /* vect_norm. */
    a.x = 100; a.y = 0;
    n = vect_norm (&a);
    test (n == 100);
    a.y = 100;
    n = vect_norm (&a);
    test (n == 141);
    a.x = 0;
    n = vect_norm (&a);
    test (n == 100);
    /* vect_from_polar_uf016. */
    vect_from_polar_uf016 (&a, 141, G_ANGLE_UF016_DEG (0));
    test (a.x == 141 && a.y == 0);
    vect_from_polar_uf016 (&a, 141, G_ANGLE_UF016_DEG (45));
    test (a.x == 100 && a.y == 100);
    vect_from_polar_uf016 (&a, 141, G_ANGLE_UF016_DEG (90));
    test (a.x == 0 && a.y == 141);
    vect_from_polar_uf016 (&a, 141, G_ANGLE_UF016_DEG (180));
    test (a.x == -141 && a.y == 0);
    /* vect_normal. */
    a.x = 10; a.y = 20;
    vect_normal (&a);
    test (a.x == -20 && a.y == 10);
    vect_normal (&a);
    test (a.x == -10 && a.y == -20);
    vect_normal (&a);
    test (a.x == 20 && a.y == -10);
    vect_normal (&a);
    test (a.x == 10 && a.y == 20);
    /* vect_dot_product. */
    a.x = 100; a.y = 0;
    b.x = 100; b.y = 0;
    p = vect_dot_product (&a, &b);
    test (p == 10000);
    a.x = 100; a.y = 0;
    b.x = 0; b.y = 100;
    p = vect_dot_product (&a, &b);
    test (p == 0);
    a.x = -100; a.y = 0;
    b.x = 100; b.y = 0;
    p = vect_dot_product (&a, &b);
    test (p == -10000);
    vect_from_polar_uf016 (&a, 100, G_ANGLE_UF016_DEG (0));
    vect_from_polar_uf016 (&b, 100, G_ANGLE_UF016_DEG (60));
    p = vect_dot_product (&a, &b);
    test (p == 5000);
    a.x = 30000; a.y = 0;
    b.x = 30000; b.y = 0;
    p = vect_dot_product (&a, &b);
    test (p == 900000000l);
    /* vect_array_*. */
    vect_t c[4] =
      {
	  { 282, 0 },
	  { 200, 200 },
	  { 0, 282 },
	  { -200, 200 },
      };
    vect_array_rotate_uf016 (c, UTILS_COUNT (c), G_ANGLE_UF016_DEG (135));
    vect_array_scale_f824 (c, UTILS_COUNT (c), 0x400000);
    a.x = 60; a.y = 20;
    vect_array_translate (c, UTILS_COUNT (c), &a);
    test (c[0].x == -50 + 60 && c[0].y == 50 + 20);
    test (c[1].x == -70 + 60 && c[1].y == 0 + 20);
    test (c[2].x == -50 + 60 && c[2].y == -50 + 20);
    test (c[3].x == 0 + 60 && c[3].y == -70 + 20);
}

int
main (void)
{
    uart0_init ();
    test_geometry ();
    test_vect ();
    uart0_putc ('o');
    uart0_putc ('k');
    uart0_putc ('\n');
    return 0;
}
