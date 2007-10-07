/* utils.host.c */
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
#include "utils.h"
#include "modules/host/host.h"

#include <time.h>
#include <errno.h>

/** Delay in seconds. */
void
utils_delay (double s)
{
    struct timespec ts;
    assert (s > 0.0);
    ts.tv_sec = (long int) s;
    ts.tv_nsec = (long int) (s - (long int) s) * 1000000000l;
    while (nanosleep (&ts, &ts) == -1 && errno == EINTR)
	;
}

/** Reset. */
void
utils_reset (void)
{
    host_reset ();
}

