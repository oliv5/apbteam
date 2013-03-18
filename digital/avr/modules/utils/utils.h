#ifndef utils_h
#define utils_h
/* utils.h */
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

#if defined (TARGET_avr)
# include "utils.avr.h"
#else
# include "utils.host.h"
#endif

/** Shortcut for delay (ns * 1e-9). */
extern inline void
utils_delay_ns (double ns) __attribute__ ((always_inline));
extern inline void
utils_delay_ns (double ns)
{
    utils_delay (ns * 1e-9);
}

/** Shortcut for delay (us * 1e-6). */
extern inline void
utils_delay_us (double us) __attribute__ ((always_inline));
extern inline void
utils_delay_us (double us)
{
    utils_delay (us * 1e-6);
}

/** Shortcut for delay (ms * 1e-3). */
extern inline void
utils_delay_ms (double ms) __attribute__ ((always_inline));
extern inline void
utils_delay_ms (double ms)
{
    utils_delay (ms * 1e-3);
}

/** Bound a value between MIN and MAX. */
#define UTILS_BOUND(v, min, max) \
    do { \
	if ((v) < (min)) \
	    (v) = (min); \
	else if ((v) > (max)) \
	    (v) = (max); \
    } while (0)

/** Return maximum of two values. */
#define UTILS_MAX(a, b) ((a) > (b) ? (a) : (b))

/** Return minimum of two values. */
#define UTILS_MIN(a, b) ((a) < (b) ? (a) : (b))

/** Absolute value. */
#define UTILS_ABS(v) ((v) > 0 ? (v) : -(v))

/** Count the number of element in an array. */
#define UTILS_COUNT(a) (sizeof (a) / sizeof ((a)[0]))

static inline uint8_t
lesseq_mod8 (uint8_t a, uint8_t b)
{
    return ((int8_t) (a - b)) <= 0;
}

#endif /* utils_h */
