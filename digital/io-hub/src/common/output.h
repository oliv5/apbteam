#ifndef output_h
#define output_h
/* output.h */
/* io-hub - Modular Input/Output. {{{
 *
 * Copyright (C) 2012 Nicolas Schodet
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
#include "simu.host.h"
#include "io.h"

#include "output_defs.h"

/** To set an output, use IO_SET (OUTPUT_EXAMPLE) or to clear an output, use
 * IO_CLR (OUTPUT_EXAMPLE). */

/** Initialize output module. */
void
output_init (void);

/** Set state of any number of output. */
void
output_set (uint32_t set);

/** Clear state of any number of output. */
void
output_clear (uint32_t clear);

#ifdef HOST

/** On host, initialise host part. */
void
output_host_init (void);

/** On host, update output state. */
void
output_host_update (void);

#endif

#endif /* output_h */
