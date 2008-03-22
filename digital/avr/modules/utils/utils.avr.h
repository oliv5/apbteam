#ifndef utils_avr_h
#define utils_avr_h
/* utils.avr.h */
/* avr.utils - Utilities AVR module. {{{
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

#include <avr/wdt.h>

/** Helper macro to build register value.  Call it like this :
 * 
 * TCCR0 = regv (FOC0, WGM00, COM01, COM0, WGM01, CS02, CS01, CS00,
 *               0,     0,     0,    0,     0,    1,    1,    1);
 *
 * The macro check that the bits are given in the right order and compute the
 * register value.  If the bits are false, the compiler will generate an
 * warning about a constant being to large.
 */
#define regv(b7, b6, b5, b4, b3, b2, b1, b0, v7, v6, v5, v4, v3, v2, v1, v0) \
    ((b7) == 7 && (b6) == 6 && (b5) == 5 && (b4) == 4 \
     && (b3) == 3 && (b2) == 2 && (b1) == 1 && (b0) == 0 \
     ? (v7) << 7 | (v6) << 6 | (v5) << 5 | (v4) << 4 \
     | (v3) << 3 | (v2) << 2 | (v1) << 1 | (v0) << 0 \
     : -1024 * 1024)

/** Helper macro to set a bit to 1 in a byte. */
#define set_bit(port, bit) (port |= _BV(bit))

/** No-operation, one instruction delay. */
extern inline void
utils_nop (void)
{
    asm volatile ("nop" : :);
}

/** Delay in seconds.  Do not call this function with a variable parameter.
 * If you want a variable delay, prefer looping over a fixed delay.
 * Maximum is about 4 seconds at 20MHz. */
extern inline void
utils_delay (double s) __attribute__ ((always_inline));

extern inline void
utils_delay (double s)
{
    double cycles = (double) AC_FREQ * s;
    if (cycles <= 1.0)
      {
	/* Delay shorter than 1 instruction. */
	asm volatile ("nop" : :);
      }
    else if (cycles <= 2.0)
      {
	/* Delay shorter than 2 instruction. */
	asm volatile ("nop\n\tnop" : :);
      }
    else if (cycles <= 3.0)
      {
	/* Delay shorter than 3 instruction. */
	asm volatile ("nop\n\tnop\n\tnop" : :);
      }
    else if (cycles <= 255 * 3)
      {
	uint8_t i = cycles / 3;
	asm volatile ("1: dec %0\n\tbrne 1b" : "=r" (i) : "0" (i));
      }
    else if (cycles <= 65535 * 4)
      {
	uint16_t i = cycles / 4;
	asm volatile ("1: sbiw %0,1\n\tbrne 1b" : "=w" (i) : "0" (i));
      }
    else
      {
	uint8_t j = (cycles + (65535 * 4 - 1)) / (65535 * 4);
	uint16_t iv = cycles / (j * 4);
	while (j--)
	  {
	    uint16_t i = iv;
	    asm volatile ("1: sbiw %0,1\n\tbrne 1b" : "=w" (i) : "0" (i));
	  }
      }
}

extern inline void
utils_reset (void) __attribute__ ((noreturn));

/** Reset the avr using the watchdog. */
extern inline void
utils_reset (void)
{
    wdt_enable (WDTO_15MS);
    while (1)
	;
}

#endif /* utils_avr_h */
