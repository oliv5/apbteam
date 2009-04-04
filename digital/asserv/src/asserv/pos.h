#ifndef pos_h
#define pos_h
/* pos.h */
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

/** Position control state. */
struct pos_t
{
    /** Current position. */
    uint32_t cur;
    /** Consign position. */
    uint32_t cons;
    /** PID coefficients (f8.8, maximum depends on saturation values). */
    uint16_t kp, ki, kd;
    /** Current integral value. */
    int32_t i;
    /** Last error value. */
    int32_t e_old;
    /** Old current position. */
    uint32_t cur_old;
    /** Count the number of blocked detection. */
    uint8_t blocked_counter;
};

extern struct pos_t pos_theta, pos_alpha;
extern struct pos_t pos_aux[AC_ASSERV_AUX_NB];

extern int32_t pos_e_sat, pos_i_sat, pos_d_sat;
extern int32_t pos_blocked_error_limit, pos_blocked_speed_limit,
       pos_blocked_counter_limit;

void
pos_update (void);

void
pos_reset (struct pos_t *pos);

#endif /* pos_h */
