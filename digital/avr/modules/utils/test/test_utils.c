/* test_utils.c */
/* avr.utils - Utilities AVR module. {{{
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
#include "modules/utils/utils.h"
#include "io.h"

int
main (int argc, char **argv)
{
    avr_init (argc, argv);
#ifndef HOST
    /* Test regv, the first one should generate a warning. */
    TCCR0 = regv (FOC0, WGM00, COM01, COM0, WGM01, CS02, CS00, CS01,
		     0,     0,     0,    0,     0,    1,    1,    1);
    TCCR0 = regv (FOC0, WGM00, COM01, COM0, WGM01, CS02, CS01, CS00,
		     0,     0,     0,    0,     0,    1,    1,    1);
#endif
    /* Test delays, use sei to separate loops in assembly listing. */
    sei (); utils_delay_ns (1);
    sei (); utils_delay_ns (1000);
    sei (); utils_delay_ns (1000000);
    sei (); utils_delay (1);
    sei (); utils_delay (4);
    sei ();
    /* Same player, play again. */
    utils_reset ();
}
