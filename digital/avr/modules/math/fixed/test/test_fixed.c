/* test_fixed.c */
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
#include "modules/math/fixed/fixed.h"
#include "modules/math/random/random.h"
#include "modules/uart/uart.h"
#include "modules/proto/proto.h"
#include "modules/utils/utils.h"
#include "io.h"

#ifdef HOST

#include <math.h>
#include <fenv.h>
#include <stdio.h>

static void
check_mul (int32_t a, int32_t b, int32_t r)
{
    feclearexcept (FE_ALL_EXCEPT);
    double af = (double) a / (1 << 24);
    double bf = (double) b / (1 << 24);
    int32_t ri = rint (af * bf * (1 << 24));
    if (r != ri && !fetestexcept (FE_INVALID))
	printf ("error: %08x * %08x != %08x (%08x)\n", a, b, r, ri);
}

static void
check_div (int32_t a, int32_t b, int32_t r)
{
    feclearexcept (FE_ALL_EXCEPT);
    double af = (double) a / (1 << 24);
    double bf = (double) b / (1 << 24);
    int32_t ri = rint (af / bf * (1 << 24));
    if (r != ri && !fetestexcept (FE_INVALID))
	printf ("error: %08x / %08x != %08x (%08x)\n", a, b, r, ri);
}

static void
check_cos (int32_t a, int32_t rc, int32_t rs)
{
    double af = (double) a / (1 << 24) * 2 * M_PI;
    int32_t rci = cos (af) * (1 << 24);
    if (rc != rci)
	printf ("error: cos (%08x) != %08x (%08x%+d)\n",
		a, rc, rci, rc - rci);
    int32_t rsi = sin (af) * (1 << 24);
    if (rs != rsi)
	printf ("error: sin (%08x) != %08x (%08x%+d)\n",
		a, rs, rsi, rs - rsi);
}

static void
check_sqrt (uint32_t a, uint32_t rf, uint16_t ri)
{
    double aff = (double) a / (1 << 8);
    uint32_t rfi = sqrt (aff) * (1 << 8);
    if (rf != rfi)
	printf ("error: sqrt_f (%08x) != %08x (%08x%+d)\n",
		a, rf, rfi, rf - rfi);
    double aif = (double) a;
    uint16_t rii = sqrt (aif);
    if (ri != rii)
	printf ("error: sqrt_i (%08x) != %04x (%04x%+d)\n",
		a, ri, rii, ri - rii);
}

#else

# define check_mul(a, b, r)
# define check_div(a, b, r)
# define check_cos(a, rc, rs)
# define check_sqrt(a, rf, ri)

#endif

void
proto_callback (uint8_t cmd, uint8_t size, uint8_t *args)
{
    uint8_t ap, bp, as, bs;
    uint16_t i;
    int32_t al, bl, rl[4];
    uint32_t patl[] = { 0xa66a6aa6, 0x5a5affff, 0xffcdffcd, 0xffffffff };
#define patn (sizeof (patl) / sizeof (patl[0]))
#define c(cmd, size) (cmd << 8 | size)
    switch (c (cmd, size))
      {
      case c ('z', 0):
	utils_reset ();
	break;
      case c ('m', 0):
	for (ap = 0; ap < patn; ap++)
	for (bp = 0; bp < patn; bp++)
	for (as = 0; as < 32; as++)
	for (bs = 0; bs < 32; bs++)
	  {
	    al = patl[ap] >> as;
	    bl = patl[bp] >> bs;
	    proto_send2d ('a', al, bl);
	    rl[0] = fixed_mul_f824 (al, bl);
	    check_mul (al, bl, rl[0]);
	    rl[1] = fixed_mul_f824 (-al, bl);
	    check_mul (-al, bl, rl[1]);
	    rl[2] = fixed_mul_f824 (al, -bl);
	    check_mul (al, -bl, rl[2]);
	    rl[3] = fixed_mul_f824 (-al, -bl);
	    check_mul (-al, -bl, rl[3]);
	    proto_send4d ('r', rl[0], rl[1], rl[2], rl[3]);
	  }
	for (i = 0; i < 64000; i++)
	  {
	    al = random_u32 ();
	    bl = random_u32 ();
	    proto_send2d ('a', al, bl);
	    rl[0] = fixed_mul_f824 (al, bl);
	    check_mul (al, bl, rl[0]);
	    rl[1] = fixed_mul_f824 (-al, bl);
	    check_mul (-al, bl, rl[1]);
	    rl[2] = fixed_mul_f824 (al, -bl);
	    check_mul (al, -bl, rl[2]);
	    rl[3] = fixed_mul_f824 (-al, -bl);
	    check_mul (-al, -bl, rl[3]);
	    proto_send4d ('r', rl[0], rl[1], rl[2], rl[3]);
	  }
	break;
      case c ('d', 0):
	for (ap = 0; ap < patn; ap++)
	for (bp = 0; bp < patn; bp++)
	for (as = 0; as < 32; as++)
	for (bs = 0; bs < 31; bs++)
	  {
	    al = patl[ap] >> as;
	    bl = patl[bp] >> bs;
	    proto_send2d ('a', al, bl);
	    rl[0] = fixed_div_f824 (al, bl);
	    check_div (al, bl, rl[0]);
	    rl[1] = fixed_div_f824 (-al, bl);
	    check_div (-al, bl, rl[1]);
	    rl[2] = fixed_div_f824 (al, -bl);
	    check_div (al, -bl, rl[2]);
	    rl[3] = fixed_div_f824 (-al, -bl);
	    check_div (-al, -bl, rl[3]);
	    proto_send4d ('r', rl[0], rl[1], rl[2], rl[3]);
	  }
	for (i = 0; i < 64000; i++)
	  {
	    al = random_u32 ();
	    bl = random_u32 ();
	    if (bl != 0)
	      {
		proto_send2d ('a', al, bl);
		rl[0] = fixed_div_f824 (al, bl);
		check_div (al, bl, rl[0]);
		rl[1] = fixed_div_f824 (-al, bl);
		check_div (-al, bl, rl[1]);
		rl[2] = fixed_div_f824 (al, -bl);
		check_div (al, -bl, rl[2]);
		rl[3] = fixed_div_f824 (-al, -bl);
		check_div (-al, -bl, rl[3]);
		proto_send4d ('r', rl[0], rl[1], rl[2], rl[3]);
	      }
	  }
	break;
      case c ('c', 0):
	for (al = 0; al < (1L << 24); al += 257)
	  {
	    proto_send1d ('a', al);
	    rl[0] = fixed_cos_f824 (al);
	    rl[1] = fixed_sin_f824 (al);
	    check_cos (al, rl[0], rl[1]);
	    proto_send2d ('r', rl[0], rl[1]);
	  }
	for (i = 0; i < 64000; i++)
	  {
	    al = random_u32 () & 0xffffff;
	    proto_send1d ('a', al);
	    rl[0] = fixed_cos_f824 (al);
	    rl[1] = fixed_sin_f824 (al);
	    check_cos (al, rl[0], rl[1]);
	    proto_send2d ('r', rl[0], rl[1]);
	  }
	break;
      case c ('s', 0):
	for (ap = 0; ap < patn; ap++)
	for (as = 0; as < 32; as++)
	  {
	    al = patl[ap] >> as;
	    proto_send1d ('a', al);
	    rl[0] = fixed_sqrt_uf248 (al);
	    rl[1] = fixed_sqrt_ui32 (al);
	    check_sqrt (al, rl[0], rl[1]);
	    proto_send2d ('r', rl[0], rl[1]);
	  }
	for (i = 0; i < 64000; i++)
	  {
	    al = random_u32 ();
	    proto_send1d ('a', al);
	    rl[0] = fixed_sqrt_uf248 (al);
	    rl[1] = fixed_sqrt_ui32 (al);
	    check_sqrt (al, rl[0], rl[1]);
	    proto_send2d ('r', rl[0], rl[1]);
	  }
	break;
      default:
	proto_send0 ('?');
	return;
      }
    /* When no error acknoledge. */
    proto_send (cmd, size, args);
#undef c
}

int
main (int argc, char **argv)
{
    avr_init (argc, argv);
    sei ();
    uart0_init ();
    proto_send0 ('z');
    while (1)
      {
	uint8_t c = uart0_getc ();
	proto_accept (c);
      }
}
