#ifndef output_defs_h
#define output_defs_h
/* output_defs.h */
/* guybrush - Eurobot 2012 AI. {{{
 *
 * Copyright (C) 2012 Nicolas Schodet
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

#define OUTPUT_PNEUM_OPEN A, 5
#define OUTPUT_UPPER_CLAMP_OPEN C, 1
#define OUTPUT_UPPER_CLAMP_OUT C, 3
#define OUTPUT_UPPER_CLAMP_IN C, 5
#define OUTPUT_UPPER_CLAMP_DOWN C, 2
#define OUTPUT_UPPER_CLAMP_UP C, 4
#define OUTPUT_DOOR_OPEN C, 7
#define OUTPUT_DOOR_CLOSE C, 6
#define OUTPUT_LOWER_CLAMP_1_OPEN D, 4
#define OUTPUT_LOWER_CLAMP_2_OPEN B, 5

#define OUTPUT_LIST \
    OUTPUT (OUTPUT_PNEUM_OPEN, 0) \
    OUTPUT (OUTPUT_UPPER_CLAMP_OPEN, 0) \
    OUTPUT (OUTPUT_UPPER_CLAMP_OUT, 0) \
    OUTPUT (OUTPUT_UPPER_CLAMP_IN, 0) \
    OUTPUT (OUTPUT_UPPER_CLAMP_DOWN, 0) \
    OUTPUT (OUTPUT_UPPER_CLAMP_UP, 0) \
    OUTPUT (OUTPUT_DOOR_OPEN, 0) \
    OUTPUT (OUTPUT_DOOR_CLOSE, 0) \
    OUTPUT (OUTPUT_LOWER_CLAMP_1_OPEN, 0) \
    OUTPUT (OUTPUT_LOWER_CLAMP_2_OPEN, 0) \

#endif /* output_defs_h */
