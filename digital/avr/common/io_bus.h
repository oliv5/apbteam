#ifndef io_bus_h
#define io_bus_h
/* io_bus.h */
/* avr.modules - AVR modules. {{{
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
#include "preproc.h"
#include "io.h"

/** Macro to work with buses of several AVR IO.
 *
 * A bus is defined by one or several IO blocks.  Each IO block is defined by
 * its port, width and shift.  Here is an example:
 *
 *  A, 4, 0, B, 4, 2
 *
 * This defines a bus composed of two blocks.  The first block is composed of
 * the first four IO from port A, and the second one is composed of the IO 2,
 * 3, 4 and 5.  The bus have the following mapping:
 *
 *  0 => A0 (LSB)
 *  1 => A1
 *  2 => A2
 *  3 => A3
 *  4 => B2
 *  5 => B3
 *  6 => B4
 *  7 => B5 (MSB)
 *
 * Once a bus is defined, it can be accessed as a whole using macros from this
 * file:
 *
 *  IO_BUS_OUPUT (MY_BUS);
 *  IO_BUS_SET (MY_BUS, 42);
 *  IO_BUS_SET (MY_BUS, 0);
 *  IO_BUS_INPUT (MY_BUS)
 *  v = IO_BUS_GET (MY_BUS);
 *
 * Width and shift must expand to a single integer.  Currently limited to 3
 * blocks, that can be easily changed.  Total bus width is limited to 8 bit
 * (because of uint8_t temporary storage needed for right shift). */

/* Warning: C preprocessor tricks inside. */

/* Internal: Return a bit mask for the given width. */
#define IO_BUS_ONES(w) ((1u << (w)) - 1)

/* Internal: Return a bit mask for the given width and shift. */
#define IO_BUS_MASK(w, s) (IO_BUS_ONES ((w)) << (s))

/* Internal: Replace bits in old 8 bit register with value from new at the
 * given width and shift.  This is done to optimize the case when the whole
 * register is replaced and does not have to be read (the compiler can not
 * optimize that because registers are volatile).  Width must expand to a
 * single integer. */
#define IO_BUS_REPLACE(old, new, w, s) \
    PREPROC_PASTE (IO_BUS_REPLACE_, w) ((old), (new), (s))
#define IO_BUS_REPLACE_n(old, new, w, s) \
	(((old) & ~IO_BUS_MASK ((w), (s))) \
	 | (((new) & IO_BUS_ONES ((w))) << (s)))
#define IO_BUS_REPLACE_1(old, new, s) IO_BUS_REPLACE_n ((old), (new), 1, (s))
#define IO_BUS_REPLACE_2(old, new, s) IO_BUS_REPLACE_n ((old), (new), 2, (s))
#define IO_BUS_REPLACE_3(old, new, s) IO_BUS_REPLACE_n ((old), (new), 3, (s))
#define IO_BUS_REPLACE_4(old, new, s) IO_BUS_REPLACE_n ((old), (new), 4, (s))
#define IO_BUS_REPLACE_5(old, new, s) IO_BUS_REPLACE_n ((old), (new), 5, (s))
#define IO_BUS_REPLACE_6(old, new, s) IO_BUS_REPLACE_n ((old), (new), 6, (s))
#define IO_BUS_REPLACE_7(old, new, s) IO_BUS_REPLACE_n ((old), (new), 7, (s))
#define IO_BUS_REPLACE_8(old, new, s) (new)

/** Read value from bus. */
#define IO_BUS_GET(bus) \
    PREPROC_NARG_CALL (IO_BUS_GET_, bus)
#define IO_BUS_GET_3(p0, w0, s0) \
    ((PIN ## p0 >> (s0)) & IO_BUS_ONES ((w0)))
#define IO_BUS_GET_6(p0, w0, s0, p1, w1, s1) \
    (IO_BUS_GET_3 (p0, (w0), (s0)) \
     | (IO_BUS_GET_3 (p1, (w1), (s1)) << (w0)))
#define IO_BUS_GET_9(p0, w0, s0, p1, w1, s1, p2, w2, s2) \
    (IO_BUS_GET_6 (p0, (w0), (s0), p1, (w1), (s1)) \
     | (IO_BUS_GET_3 (p2, (w2), (s2)) << ((w0) + (w1))))

/** Write value to bus. */
#define IO_BUS_SET(bus, value) \
    PREPROC_NARG_CALL (IO_BUS_SET_, bus, (value))
#define IO_BUS_SET_4(p0, w0, s0, value) \
    do { \
	uint8_t _value1 = (value); \
	PORT ## p0 = IO_BUS_REPLACE (PORT ## p0, _value1, w0, (s0)); \
    } while (0)
#define IO_BUS_SET_7(p0, w0, s0, p1, w1, s1, value) \
    do { \
	uint8_t _value2 = (value); \
	IO_BUS_SET_4 (p0, w0, (s0), _value2); \
	IO_BUS_SET_4 (p1, w1, (s1), _value2 >> (w0)); \
    } while (0)
#define IO_BUS_SET_10(p0, w0, s0, p1, w1, s1, p2, w2, s2, value) \
    do { \
	uint8_t _value3 = (value); \
	IO_BUS_SET_7 (p0, w0, (s0), p1, w1, (s1), _value3); \
	IO_BUS_SET_4 (p2, w2, (s2), _value3 >> ((w0) + (w1))); \
    } while (0)

/** Set bus as input. */
#define IO_BUS_INPUT(bus) \
    PREPROC_NARG_CALL (IO_BUS_INPUT_, bus)
#define IO_BUS_INPUT_3(p0, w0, s0) \
    do { \
	DDR ## p0 = IO_BUS_REPLACE (DDR ## p0, 0, w0, (s0)); \
    } while (0)
#define IO_BUS_INPUT_6(p0, w0, s0, p1, w1, s1) \
    do { \
	IO_BUS_INPUT_3 (p0, w0, (s0)); \
	IO_BUS_INPUT_3 (p1, w1, (s1)); \
    } while (0)
#define IO_BUS_INPUT_9(p0, w0, s0, p1, w1, s1, p2, w2, s2) \
    do { \
	IO_BUS_INPUT_6 (p0, w0, (s0), p1, w1, (s1)); \
	IO_BUS_INPUT_3 (p2, w2, (s2)); \
    } while (0)

/** Set bus as output. */
#define IO_BUS_OUTPUT(bus) \
    PREPROC_NARG_CALL (IO_BUS_OUTPUT_, bus)
#define IO_BUS_OUTPUT_3(p0, w0, s0) \
    do { \
	DDR ## p0 = IO_BUS_REPLACE (DDR ## p0, IO_BUS_ONES (w0), w0, (s0)); \
    } while (0)
#define IO_BUS_OUTPUT_6(p0, w0, s0, p1, w1, s1) \
    do { \
	IO_BUS_OUTPUT_3 (p0, w0, (s0)); \
	IO_BUS_OUTPUT_3 (p1, w1, (s1)); \
    } while (0)
#define IO_BUS_OUTPUT_9(p0, w0, s0, p1, w1, s1, p2, w2, s2) \
    do { \
	IO_BUS_OUTPUT_6 (p0, w0, (s0), p1, w1, (s1)); \
	IO_BUS_OUTPUT_3 (p2, w2, (s2)); \
    } while (0)

#endif /* io_bus_h */
