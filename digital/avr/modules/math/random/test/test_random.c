/* test_random.c */
/* avr.math.random - Pseudo random numbers generator. {{{
 *
 * Copyright (C) 2005 Nicolas Schodet
 *
 * Robot APB Team/Efrei 2006.
 *        Web: http://assos.efrei.fr/robot/
 *      Email: robot AT efrei DOT fr
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
#include "modules/math/random/random.h"
#include "modules/uart/uart.h"
#include "modules/proto/proto.h"
#include "io.h"

void
proto_callback (uint8_t cmd, uint8_t size, uint8_t *args)
{
}

int
main (void)
{
    sei ();
    uart0_init ();
    proto_send0 ('z');
    while (42)
	proto_send1d ('r', random_u32 ());
}
