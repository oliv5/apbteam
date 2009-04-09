/* select.c - Output selection mechanism. */
/* dev2 - Multi-purpose development board using USB and Ethernet. {{{
 *
 * Copyright (C) 2009 Nicolas Schodet
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

#include "io.h"

#include "select.h"

#define ENABLE _BV (2)
#define SELA _BV (5)
#define SELB _BV (4)

/* Initialise selector. */
void
select_init (void)
{
    PORTC &= ~(ENABLE | SELA | SELB);
    DDRC |= ENABLE | SELA | SELB;
}

/* Select an output (1-4) or switch off (0, anything else). */
void
select_out (uint8_t num)
{
    uint8_t p = PORTC & ~(ENABLE | SELA | SELB);
    switch (num)
      {
      case 1:
	PORTC = p | ENABLE;
	break;
      case 2:
	PORTC = p | ENABLE | SELA;
	break;
      case 3:
	PORTC = p | ENABLE | SELB;
	break;
      case 4:
	PORTC = p | ENABLE | SELA | SELB;
	break;
      default:
	PORTC = p;
	break;
      }
}
