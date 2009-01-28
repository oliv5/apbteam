/* test-flash.c */
/* avr.flash - AVR Flash SPI use. {{{
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
#include "modules/flash/flash.h"
#include <stdio.h>

int
main (void)
{
    uint8_t status;
    uint32_t addr;
    uint8_t i;

    status = flash_init ();
    printf ("Flash status : %d\n", status);

    flash_erase (FLASH_ERASE_FULL, 0);

    for (i = 0; i < 10; i++)
      {
	status = flash_init ();
	printf ("Flash status : %d\n", status);

	// Get the first sector to begin.
	addr = flash_first_sector ();
	printf ("First sector : %x\n", addr);

	flash_write (addr, 0xA);
	flash_write (addr + 1, 0xB);

	printf ("First data in the flash memory : %x\n", flash_read (addr));
	printf ("Second data in the flash memory : %x\n", flash_read (addr + 1));
      }

    return 0;
}
