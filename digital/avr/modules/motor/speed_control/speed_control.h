#ifndef speed_control_h
#define speed_control_h
/* speed_control.h */
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

#include "modules/motor/pos_control/pos_control.h"

/** Speed control state. */
struct speed_control_t
{
    /** Current speed, f16.8. */
    int32_t cur_f;
    /** Whether to use the consign position (1) or not (0). */
    uint8_t use_pos;
    /** Consign speed, f16.8. */
    int32_t cons_f;
    /** Consign position. */
    uint32_t pos_cons;
    /** Maximum speed for position consign, u15. */
    int16_t max;
    /** Slow speed, deprecated, u15. */
    int16_t slow;
    /** Acceleration, f8.8. */
    int16_t acc_f;
    /** Associated position control, to simplify function prototypes. */
    pos_control_t *pos_control;
};
typedef struct speed_control_t speed_control_t;

/** Initialise speed control. */
void
speed_control_init (speed_control_t *speed_control,
		    pos_control_t *pos_control);

/** Control speed and update position control. */
void
speed_control_update (speed_control_t *speed_control,
		      uint8_t enabled);

/** Set speed consign. Accelerate to the given speed. */
void
speed_control_set_speed (speed_control_t *speed_control, int16_t speed);

/** Set position consign offset.  Move to a position measured from the current
 * controlled position (which may be different from the actual position),
 * control speed to match requested acceleration. */
void
speed_control_pos_offset (speed_control_t *speed_control, int32_t offset);

/** Set position consign offset from the current actual position.  Move to a
 * position measured from the actual position, control speed to match
 * requested acceleration.
 *
 * Warning: This is usually not what you want.  Only use this if offset has
 * been computed from encoder. */
void
speed_control_pos_offset_from_here (speed_control_t *speed_control,
				    int32_t offset);

/** Set absolute position consign.  Useful to restore an previous position. */
void
speed_control_pos (speed_control_t *speed_control, int32_t pos_cons);

/** Stop right now, no acceleration!  You really should avoid this function
 * unless the motor is actually almost stopped. */
void
speed_control_hard_stop (speed_control_t *speed_control);

#endif /* speed_control_h */
