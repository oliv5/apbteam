#ifndef blocking_detection_h
#define blocking_detection_h
/* blocking_detection.h */
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

/** Blocking detection state. */
struct blocking_detection_t
{
    /** True if blocking is detected. */
    uint8_t blocked;
    /** Count the number of blocked detection. */
    uint8_t counter;
    /** Error limit. */
    int32_t error_limit;
    /** Speed limit. */
    int16_t speed_limit;
    /** Counter limit. */
    uint8_t counter_limit;
};
typedef struct blocking_detection_t blocking_detection_t;

/** Detect blocking, update blocking state. */
void
blocking_detection_update (blocking_detection_t *blocking_detection,
			   int16_t current_speed, int16_t output,
			   pos_control_t *pos_control,
			   uint8_t control_enabled);

#endif /* blocking_detection_h */
