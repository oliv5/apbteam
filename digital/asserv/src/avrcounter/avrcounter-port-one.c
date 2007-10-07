/* avrcounter-port-one.c - Single encoder, port output. */
/* avrcounter - Incremental encoder counter. {{{
 *
 * Copyright (C) 2007 Nicolas Schodet
 *
 * Robot APB Team 2008.
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
#include <io.h>

int
main (void)
{
    uint8_t count = 0;
    uint8_t state = 0;
    /* Encoder signals:
     * ____----____----____----
     * __----____----____----__  */
    static const int8_t inc[] = {
	0,  /* 00 -> 00 */
	+1, /* 00 -> 01 */
	-1, /* 00 -> 10 */
	0,  /* 00 -> 11, missed! */
	-1, /* 01 -> 00 */
	0,  /* 01 -> 01 */
	0,  /* 01 -> 10, missed! */
	+1, /* 01 -> 11 */
	+1, /* 10 -> 00 */
	0,  /* 10 -> 01, missed! */
	0,  /* 10 -> 10 */
	-1, /* 10 -> 11 */
	0,  /* 11 -> 00, missed! */
	-1, /* 11 -> 01 */
	+1, /* 11 -> 10 */
	0,  /* 11 -> 11 */
    };
    DDRD = 0xff;
    while (1)
      {
	state = (state << 2) | (PINC & 0x03);
	count += inc[state];
	PORTD = count;
      }
}
