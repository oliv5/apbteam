#ifndef speed_h
#define speed_h
/* speed.h */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2008 Nicolas Schodet
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

/** Speed control state. */
struct speed_t
{
    /** Current speed, f8.8. */
    int16_t cur;
    /** Consign speed, f8.8. */
    int16_t cons;
    /** Maximum speed for position consign, u8. */
    int8_t max;
    /** Slow speed for position consign, u8. */
    int8_t slow;
    /** Acceleration, f8.8. */
    int16_t acc;
    /** Consign position. */
    uint32_t pos_cons;
    /** Whether to use the consign position (1) or not (0). */
    uint8_t use_pos;
};

extern struct speed_t speed_theta, speed_alpha;

void
speed_update (void);

#endif /* speed_h */
