/* test-erase.c */
/* avr.flash - AVR Trace. {{{
 *
 * Copyright (C) 2008 Nélio Laranjeiro
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
#include "../../trace.h"
#include "modules/flash/flash.h"

#include <stdio.h>
#include "events.h"

void
flood (void)
{
    uint8_t cmd;
    uint32_t addr;
    uint32_t count;

    uint32_t speed;
    uint32_t position;
    uint16_t acc;

    uint16_t arg1;
    uint8_t arg2;
    uint32_t arg3;

    trace_init ();

    /* Flood the flash memory with traces. */
    /* A little more than 3 memory sectors, a sector is 4 kbytes. */
    for (count = 0; count < 2000; count ++)
      {
        /* Right motor. */
        speed = 10;
        position = 11;
        acc = 12;
        arg1 = 10;
        arg2 = 11;
        arg3 = 12;

	cmd = TRACE_ASSERV__RIGHT_MOTOR;
        TRACE (cmd, speed, position, acc);
	cmd = TRACE_ASSERV__LEFT_MOTOR;
        TRACE (cmd, speed, position, acc);
	cmd = TRACE_IA__IA_CMD;
        TRACE (cmd, arg1, arg2, arg3);
      }
}

void
dump (void)
{
    uint8_t status;
    uint32_t addr;

    status = flash_init ();

    for (addr = 0; addr < FLASH_ADDRESS_HIGH; addr ++)
      {
	printf ("%02x", flash_read (addr));
      }
}

int
main (void)
{
    uint8_t i;

    for (i = 0; i < 30; i++)
	flood ();

    dump();
    return 0;
}

