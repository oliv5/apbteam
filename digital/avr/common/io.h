#ifndef io_h
#define io_h
/* io.h */
/* avr.modules - AVR modules. {{{
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

#ifndef HOST

/* Avr part. */
#include <avr/interrupt.h>
#if !defined (__AVR_LIBC_VERSION__) || __AVR_LIBC_VERSION__ < 10400UL
#  include <avr/signal.h>
   /* No INTERRUPT as it is dangerous. */
#  undef INTERRUPT
   /* SIGNAL is now ISR. */
#  define ISR SIGNAL
#endif

#else /* HOST */

/* Same as on AVR. */
#define _BV(x) (1<<(x))
#define bit_is_set(sfr, bit) ((sfr) & _BV (bit))
#define bit_is_clear(sfr, bit) (!((sfr) & _BV (bit)))
/* No interrupt support on host. */
#define sei()
#define cli()

#endif /* HOST */

/** Use these macros to define port and bit number combination.
 * For example:
 *
 *   #define MY_IO B, 5
 *   IO_PORT (MY_IO) => PORTB
 *   IO_DDR (MY_IO) => DDRB
 *   IO_PIN (MY_IO) => PINB
 *   IO_BV (MY_IO) => _BV (5)
 *   IO_N (MY_IO) => (5)
 */
#define IO_PORT(io) IO_PORT_ (io)
#define IO_PORT_(p, n) PORT ## p
#define IO_DDR(io) IO_DDR_ (io)
#define IO_DDR_(p, n) DDR ## p
#define IO_PIN(io) IO_PIN_ (io)
#define IO_PIN_(p, n) PIN ## p
#define IO_BV(io) IO_BV_ (io)
#define IO_BV_(p, n) _BV (n)
#define IO_N(io) IO_N_ (io)
#define IO_N_(p, n) (n)

#endif /* io_h */
