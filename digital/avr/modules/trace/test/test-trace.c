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
#include "../trace.h"

#include <stdio.h>

int
main (void)
{
    uint8_t val1 = 0xEF;
    uint16_t val2 = 0x1234;
    uint32_t val3 = 0x456789AB;

    trace_init ();

    printf ("First trace\n");
    TRACE (val1);

    printf ("Second trace\n");
    TRACE (val2);

    printf ("Third trace\n");
    TRACE (val3);

    printf ("Fourth trace\n");
    TRACE (val1, val2);

    printf ("Fifth trace\n");
    TRACE (val1, val3);

    printf ("sixth one\n");
    TRACE (val1, val2, val3);

    printf ("last one\n");
    TRACE (val3, val1, val2);

    return 0;
}

