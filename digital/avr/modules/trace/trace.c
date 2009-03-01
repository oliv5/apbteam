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

#define TRACE_CODE_START FLASH_LOG_CODE

#define TRACE_ARGS_MAX 6
#define TRACE_MAX_ARGS (TRACE_ARGS_MAX * TRACE_ARGS_MAX)

struct trace_t
{
    /** Flash status. */
    trace_status_t status;
    /** Flash start address */
    const uint32_t addr_start;
    /** Flash address. */
    uint32_t addr;
    /** Flash next sector */
    uint32_t next_sector;
};
typedef struct trace_t trace_t;

static trace_t trace_global;

/** Erase the next sector on the Flash memory.
  */
static void
trace_erase_next_sector (void)
{
    /* If the flash is enable and the start sector is not reached yet erase
     * the sector. */
    if (trace_global.status
	&& (flash_read (trace_global.next_sector) != 0xFF))
      {
	if (trace_global.next_sector != trace_global.addr_start)
	  {
	    /* Flash page size is equal to 4k. */
	    flash_erase (FLASH_ERASE_4K, trace_global.next_sector);
	  }
	else
	    /* Disable the flash. */
	    trace_global.status = TRACE_STATUS_OFF;
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


uint8_t
trace_init (void)
{
    int8_t i;
    trace_global.status = flash_init ();

    /* Get the first sector to write. */
    if (trace_global.status)
      {
        trace_global.addr = flash_first_sector();
	*((uint32_t *) &trace_global.addr_start) =
	    FLASH_PAGE(trace_global.addr);
        trace_global.next_sector =
            FLASH_PAGE (trace_global.addr + FLASH_PAGE_SIZE);

        /* If the next sector is the first one in the memory erase it. */
	trace_erase_next_sector ();

        /* Store the start code. */
	for (i = 24; i >= 0; i -= 8)
	  {
	    flash_write (trace_global.addr,
			 TRACE_CODE_START >> i);
	    trace_global.addr =
		FLASH_ADDRESS_INC(trace_global.addr);
	  }
	return TRACE_STATUS_ON;
      }
    return TRACE_STATUS_OFF;
}

void
trace_print (uint8_t arg)
{
    /* Store the arg on flash */
    if (trace_global.status)
      {
	uint32_t curr_sector;
	flash_write (trace_global.addr, arg);
	trace_global.addr = FLASH_ADDRESS_INC(trace_global.addr);

	/* Compute the next sector address. */
	curr_sector = trace_global.next_sector;
	trace_global.next_sector = FLASH_PAGE (trace_global.addr +
						   FLASH_PAGE_SIZE);
	if (curr_sector != trace_global.next_sector)
	    trace_erase_next_sector ();
      }
}

/** Get the current status of the trace module.
  * \return  0x1 if the module is activate, 0x0 if the module is not active.
  */
trace_status_t
trace_status (void)
{
    return trace_global.status;
}

/** Get the current address.
  * \return  addr  the current address managed by the trace module.
  */
uint32_t
trace_addr_current (void)
{
    return trace_global.addr;
}
