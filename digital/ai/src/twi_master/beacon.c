/* beacon.c */
/* ai - Robot Artificial Intelligence. {{{
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
#include "common.h"
#include "beacon.h"

#include "twi_master.h"

#include "modules/utils/byte.h"

/** Global context. */
struct beacon_t
{
    /** Obstacles position. */
    vect_t position[AC_BEACON_POSITION_NB];
    /** Obstacles trust. */
    uint8_t trust[AC_BEACON_POSITION_NB];
};
struct beacon_t beacon;

void
beacon_init (void)
{
    uint8_t i;
    for (i = 0; i < AC_BEACON_POSITION_NB; i++)
	beacon.trust[i] = 0;
}

void
beacon_status_cb (uint8_t *status)
{
    uint8_t i, index = 3;
    for (i = 0; i < AC_BEACON_POSITION_NB; i++)
      {
	beacon.position[i].x = v8_to_v16 (status[index], status[index + 1]);
	index += 2;
	beacon.position[i].y = v8_to_v16 (status[index], status[index + 1]);
	index += 2;
	beacon.trust[i] = status[index];
	index++;
      }
}

void
beacon_on (uint8_t on_off)
{
    uint8_t *buffer = twi_master_get_buffer (BEACON_SLAVE);
    buffer[0] = on_off;
    twi_master_send_buffer (1);
}

uint8_t
beacon_get_position (uint8_t index, vect_t *position)
{
    if (index < AC_BEACON_POSITION_NB)
      {
	*position = beacon.position[index];
	return beacon.trust[index];
      }
    else
	return 0;
}

