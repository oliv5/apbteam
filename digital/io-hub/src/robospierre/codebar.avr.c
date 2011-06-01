/* codebar.avr.c */
/* robospierre - Eurobot 2011 AI. {{{
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
#include "codebar.h"

#include "defs.h"

#include "modules/twi/twi.h"
#include "modules/utils/utils.h"
#include "modules/utils/crc.h"
#include "modules/utils/byte.h"

#define CODEBAR_ADDRESS 0x20
#define CODEBAR_STATUS_LENGTH 7

void
codebar_init (void)
{
}

uint8_t
codebar_get (uint8_t direction)
{
    uint8_t buffer[CODEBAR_STATUS_LENGTH];
    /* Read status. */
    twi_master_recv (CODEBAR_ADDRESS, buffer, sizeof (buffer));
    uint8_t ret = twi_master_wait ();
    if (ret != CODEBAR_STATUS_LENGTH)
	return 0;
    uint8_t crc = crc_compute (buffer + 1, CODEBAR_STATUS_LENGTH - 1);
    if (crc != buffer[0])
	return 0;
    /* Get data. */
    uint8_t offset = direction == DIRECTION_FORWARD ? 1 : 4;
    uint16_t age = v8_to_v16 (buffer[offset], buffer[offset + 1]);
    uint16_t type = buffer[offset + 2];
    if (age > 3 * 250)
	return 0;
    else
	return  type;
}

