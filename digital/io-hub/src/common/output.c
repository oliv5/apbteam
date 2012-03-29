/* output.c */
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
#include "common.h"
#include "output.h"

#include "simu.host.h"

void
output_init (void)
{
#define OUTPUT(io, init) do { \
    if (init) IO_SET_ (io); \
    IO_OUTPUT_ (io); \
} while (0); // <- do not copy this code unless you know why!
    OUTPUT_LIST
#undef OUTPUT
}

void
output_set (uint32_t set)
{
#define OUTPUT(io, init) do { \
    if (set & 1ul) IO_SET_ (io); \
    set >>= 1; \
} while (0); // <- do not copy this code unless you know why!
    OUTPUT_LIST
#undef OUTPUT
}

void
output_clear (uint32_t clear)
{
#define OUTPUT(io, init) do { \
    if (clear & 1ul) IO_CLR_ (io); \
    clear >>= 1; \
} while (0); // <- do not copy this code unless you know why!
    OUTPUT_LIST
#undef OUTPUT
}

/** Toggle state of any number of output. */
void
output_toggle (uint32_t toggle)
{
#define OUTPUT(io, init) do { \
    if (toggle & 1ul) IO_TOGGLE_ (io); \
    toggle >>= 1; \
} while (0); // <- do not copy this code unless you know why!
    OUTPUT_LIST
#undef OUTPUT
}

