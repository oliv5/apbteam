/* blocking_detection.c */
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
#include "common.h"
#include "blocking_detection.h"

#include "modules/utils/utils.h"

void
blocking_detection_update (blocking_detection_t *bd,
			   int16_t current_speed, int16_t output,
			   pos_control_t *pos_control,
			   uint8_t control_enabled)
{
    if (control_enabled)
      {
	/* Test for blocking condition. */
	int32_t error = pos_control->cons - pos_control->cur;
	if (UTILS_ABS (error) > bd->error_limit
	    && UTILS_ABS (current_speed) < bd->speed_limit)
	    bd->counter++;
	else
	    bd->counter = 0;
	/* Set blocked state for detector clients. */
	bd->blocked = bd->counter > bd->counter_limit;
      }
    else
      {
	bd->blocked = 0;
	bd->counter = 0;
      }
}

