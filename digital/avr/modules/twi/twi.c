/* twi.c - Common functions for all implementations. */
/* avr.twi - TWI AVR module. {{{
 *
 * Copyright (C) 2010 Nicolas Schodet
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
#include "twi.h"

#if AC_TWI_SLAVE_ENABLE && AC_TWI_SLAVE_POLLED

# include "io.h"
# include "modules/utils/utils.h"
# include <string.h>

/** Module context. */
struct twi_poll_t
{
    /** Reception buffer. */
    uint8_t recv_buffer[AC_TWI_SLAVE_RECV_BUFFER_SIZE];
    /** Reception buffer current size. */
    volatile uint8_t recv_buffer_size;
};

/** Global context. */
static struct twi_poll_t twi_poll_global;
# define ctx twi_poll_global

/** Called on data reception from master when in polled mode instead of user
 * provided callback, interrupts are locked. */
void
twi_slave_recv_polled (const uint8_t *buffer, uint8_t size)
{
    assert (size <= AC_TWI_SLAVE_RECV_BUFFER_SIZE);
    memcpy (ctx.recv_buffer, buffer, size);
    ctx.recv_buffer_size = size;
}

uint8_t
twi_slave_poll (uint8_t *buffer, uint8_t size)
{
# if !AC_TWI_NO_INTERRUPT
    /* Lock interrupts. */
    intr_flags_t flags = intr_lock ();
# endif
    /* Copy data. */
    uint8_t recv_size = ctx.recv_buffer_size;
    size = UTILS_MIN (size, recv_size);
    memcpy (buffer, ctx.recv_buffer, size);
    /* Reset. */
    ctx.recv_buffer_size = 0;
# if !AC_TWI_NO_INTERRUPT
    /* Unlock. */
    intr_restore (flags);
# endif
    return size;
}

#endif /* AC_TWI_SLAVE_ENABLE && AC_TWI_SLAVE_POLLED */
