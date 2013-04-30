// io-hub - Modular Input/Output. {{{
//
// Copyright (C) 2013 Nicolas Schodet
//
// APBTeam:
//        Web: http://apbteam.org/
//      Email: team AT apbteam DOT org
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// }}}
#include "self_programming.hh"
#include <libopencm3/stm32/f4/flash.h>

static const uint32_t sector_addr[] = {
    // 16 KB
    0x8000000, 0x8004000, 0x8008000, 0x800c000,
    // 64 KB
    0x8010000,
    // 128 KB.
    0x8020000, 0x8040000, 0x8060000, 0x8080000, 0x80a0000, 0x80c0000,
    0x80e0000, 0x8100000,
};

static int
self_programming_sector (uint32_t addr)
{
    unsigned i;
    for (i = 0; i < lengthof (sector_addr); i++)
        if (addr == sector_addr[i])
            return i;
    return -1;
}

void
self_programming_write (uint32_t addr, const char *buf, int count)
{
    int sector = self_programming_sector (addr);
    flash_unlock ();
    if (sector != -1)
        flash_erase_sector (sector << 3, FLASH_CR_PROGRAM_X32);
    for (int i = 0; i < count; i += 4)
        flash_program_word (addr + i, *(uint32_t *) (buf + i),
                            FLASH_CR_PROGRAM_X32);
    flash_lock ();
}

