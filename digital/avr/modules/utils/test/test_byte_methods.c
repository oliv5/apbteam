/* test_byte.c */
/* n.avr.utils - AVR utilities module. {{{
 *
 * Copyright (C) 2005 Nicolas Schodet
 *
 * Robot APB Team/Efrei 2005.
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
#include "io.h"

/* +AutoDec */
/* -AutoDec */

#define nop() asm volatile ("nop" ::)

/* Union for byte access. */
union _utils_byte_access
{
    uint8_t v8[4];
    uint16_t v16[2];
    uint32_t v32;
};

int
main (void)
{
    union _utils_byte_access ba;
    volatile uint32_t d = 0;
    volatile uint8_t b0 = 1, b1 = 2, b2 = 3, b3 = 4;
    nop (); // join union method.
    ba.v8[0] = b0; ba.v8[1] = b1; ba.v8[2] = b2; ba.v8[3] = b3;
    d = ba.v32;
    nop (); // join asm method.
    asm ("mov %A0, %1"	"\r\n"
	 "mov %B0, %2"	"\r\n"
	 "mov %C0, %3"	"\r\n"
	 "mov %D0, %4"	"\r\n"
	 : "=d" (d) : "r" (b0), "r" (b1), "r" (b2), "r" (b3));
    nop (); // split union method.
    ba.v32 = d;
    b2 = ba.v8[2];
    nop (); // split asm method.
    asm ("mov %0, %A1" : "=r" (b2) : "d" (d));
    nop (); // b2 = d >> 16;
    b2 = d >> 16;
    nop (); // b2 = ((uint8_t *) &d)[2];
    b2 = ((uint8_t *) &d)[2];
    nop ();
    return 0;
}
