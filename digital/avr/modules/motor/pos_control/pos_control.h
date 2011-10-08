#ifndef pos_control_h
#define pos_control_h
/* pos_control.h */
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

/** Position control state. */
struct pos_control_t
{
    /** Current position. */
    uint32_t cur;
    /** Consign position. */
    uint32_t cons;
    /** PID coefficients (f8.8, maximum depends on saturation values). */
    uint16_t kp, ki, kd;
    /** Error saturation. */
    int32_t e_sat;
    /** Integral saturation. */
    int32_t i_sat;
    /** Differential saturation. */
    int32_t d_sat;
    /** Current integral value. */
    int32_t i;
    /** Last error value. */
    int32_t last_error;
};
typedef struct pos_control_t pos_control_t;

/** Initialise position control. */
void
pos_control_init (pos_control_t *pos_control);

/** Control position of system, take current speed as input, return the new
 * command. */
int16_t
pos_control_update (pos_control_t *pos_control, int16_t cur_speed,
		    uint8_t enabled);

#endif /* pos_control_h */
