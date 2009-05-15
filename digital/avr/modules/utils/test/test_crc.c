/* test_crc.c */
/* avr.utils - Utilities AVR module. {{{
 *
 * Copyright (C) 2009 Dufour Jérémy
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
#include "modules/utils/crc.h"

int
main (void)
{
    uint8_t test_vector[] = { 0x02, 0x1c, 0xb8, 0x01, 0, 0, 0, 0xa2 };

    if (crc_compute (test_vector,
		     sizeof test_vector / sizeof test_vector[0])
	!= 0)
	return 1;
    else
	return 0;
}
