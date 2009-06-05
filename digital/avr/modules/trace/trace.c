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
#include "modules/utils/utils.h"
#include "modules/utils/byte.h"
#include "modules/proto/proto.h"
#include "modules/flash/flash.h"
#include "trace.h"

#define TRACE_ARGS_MAX 6
#define TRACE_MAX_ARGS (TRACE_ARGS_MAX * TRACE_ARGS_MAX)

#define TRACE_BLOCK_SIZE_BYTES 65536
#define TRACE_PAGE 0x80000
#define TRACE_PAGE_BLOCKS (TRACE_PAGE / TRACE_BLOCK_SIZE_BYTES)
#define TRACE_PAGE_PAGE_NB (FLASH_SIZE / TRACE_PAGE)

struct trace_t
{
/** Flash status. */
    trace_status_t status;
    /** Flash start address */
    uint32_t addr_start;
    /** Flash address. */
    uint32_t addr;
};
typedef struct trace_t trace_t;

static trace_t trace_global;

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

static inline void
trace_erase_page (uint32_t addr)
{
    uint8_t i;
    while (flash_is_busy ());
    for (i = 0; i < TRACE_PAGE_BLOCKS; i++)
      {
	flash_erase (FLASH_ERASE_64K, addr);
	addr += TRACE_BLOCK_SIZE_BYTES;
      }
}

uint8_t
trace_init (void)
{
    int8_t i;
    uint8_t new_trace_val = 0x0;
    uint32_t new_trace_addr = 0;

    trace_global.status = flash_init ();

    /* Get the first sector to write. */
    if (trace_global.status)
      {
	uint8_t val = 0;
	/* Find the possible traces. */
	for (i = 0; i < TRACE_PAGE_PAGE_NB; i++)
	  {
	    val = flash_read (i * TRACE_PAGE);
	    if (i == 0 || lesseq_mod8(new_trace_val, val))
	      {
		new_trace_val = val;
		new_trace_addr = i * TRACE_PAGE;
	      }
	  }
	new_trace_addr &= FLASH_ADDRESS_HIGH;

	/* Flash not empty */
	if (!((new_trace_val == 0x0) && (new_trace_addr == 0)))
	  {
	    new_trace_addr = (new_trace_addr + TRACE_PAGE)
		& FLASH_ADDRESS_HIGH;

	    /* Erase it. */
	    trace_erase_page (new_trace_addr);
	  }
	new_trace_val ++;
	*((uint32_t*) &trace_global.addr_start) = new_trace_addr;

	/* Store the trace val. */
	flash_write (new_trace_addr, new_trace_val);
	trace_global.addr = new_trace_addr + 1;

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
	flash_write (trace_global.addr, arg);
	trace_global.addr ++;

	if (trace_global.addr == (trace_global.addr_start + TRACE_PAGE))
	    trace_global.status = TRACE_STATUS_OFF;
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
