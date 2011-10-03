#ifndef output_h
#define output_h
/* output.h */
/* motor - Motor control module. {{{
 *
 * Copyright (C) 2011 Nicolas Schodet
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

/** Absolute maximum output value for any output. */
#define OUTPUT_MAX 0x3ff

/** Number of defined outputs. */
#define OUTPUT_NB PREPROC_NARG (AC_OUTPUT_LIST)

/** Output state. */
struct output_t
{
    /** Current value. */
    int16_t cur;
    /** Maximum value. */
    int16_t max;
    /** Minimum value (dead zone). */
    int16_t min;
    /** Reverse this output. */
    uint8_t reverse;
};
typedef struct output_t output_t;

/** Initialize an output and attach it to provided output structure. */
void
output_init (uint8_t index, output_t *output);

/** Set output value. */
void
output_set (output_t *output, int16_t value);

/** Set output reverse flag. */
extern inline void
output_set_reverse (output_t *output, uint8_t reverse)
{
    output->reverse = reverse;
}

/** Update output value in hardware. */
void
output_update (void);

#endif /* output_h */
