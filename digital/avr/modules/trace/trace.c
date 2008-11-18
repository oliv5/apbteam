/* trace.c */
/*  {{{
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
#include "modules/utils/byte.h"
#include "modules/flash/flash.h"
#include "trace.h"

#define TRACE_CODE_START 0xF33FF22F

#define TRACE_ARGS_MAX 6
#define TRACE_MAX_ARGS (TRACE_ARGS_MAX * TRACE_ARGS_MAX)

struct trace_t
{
    /** Flash status. */
    uint8_t flash_status;
    /** Flash address. */
    uint32_t flash_addr;
    /** Flash next sector */
    uint32_t flash_next_sector;
};
typedef struct trace_t trace_t;

static trace_t trace_global;


/** Verify the stat of the next sector.
  * If the next sector is not empty it shall send an erase command on the
  * sector of the flash memory.
  */
static void
trace_next_sector_prepare (void)
{
    uint32_t addr_next;
    uint8_t data;
    if (trace_global.flash_status)
      {
	addr_next = FLASH_PAGE(trace_global.flash_addr) + FLASH_PAGE_SIZE;
	data = flash_read (addr_next);

	if (data != 0XFF)
	    flash_erase (FLASH_ERASE_4K, addr_next);
      }
}

void
trace_print_arg_1(uint8_t arg)
{
    trace_print (arg);
}

void
trace_print_arg_2(uint16_t arg)
{
    trace_print (arg >> 8);
    trace_print (arg);
}

void
trace_print_arg_4(uint32_t arg)
{
    trace_print (arg >> 24);
    trace_print (arg >> 16);
    trace_print (arg >> 8);
    trace_print (arg);
}


void
trace_init (void)
{
    uint8_t i;
    trace_global.flash_status = flash_init ();

    /* Get the first sector to write. */
    if (trace_global.flash_status)
      {
	trace_global.flash_addr = flash_sector_next (0);
	trace_global.flash_next_sector =
	    FLASH_PAGE (trace_global.flash_addr + FLASH_PAGE_SIZE);

	/* If the next sector is not empty erase it. */
	trace_next_sector_prepare ();

	/* Store the start code. */
	for (i = 0; i < 4; i ++)
	  {
	    flash_write (trace_global.flash_addr,
			 v32_to_v8(TRACE_CODE_START, i));
	    trace_global.flash_addr =
		FLASH_ADDRESS_INC(trace_global.flash_addr);
	  }
      }
}

void
trace_print (uint8_t arg)
{
    /* Store the arg on flash */
    if (trace_global.flash_status)
      {
	flash_write (trace_global.flash_addr, arg);
	trace_global.flash_addr = FLASH_ADDRESS_INC(trace_global.flash_addr);
      }

    if ((trace_global.flash_next_sector - trace_global.flash_addr)
	< TRACE_MAX_ARGS)
	trace_next_sector_prepare ();
}

/** Get the current status of the trace module.
  * \return  0x1 if the module is activate, 0x0 if the module is not active.
  */
uint8_t
trace_status (void)
{
    return trace_global.flash_status;
}

/** Get the current address.
  * \return  addr  the current address managed by the trace module.
  */
uint32_t
trace_addr_current (void)
{
    return trace_global.flash_addr;
}
