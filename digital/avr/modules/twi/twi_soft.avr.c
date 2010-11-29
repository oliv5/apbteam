/* twi_soft.avr.c - Software master implementation. */
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

#if TWI_DRIVER == TWI_DRIVER_SOFT

# include "modules/utils/utils.h"
# include "io.h"

# if AC_TWI_SLAVE_ENABLE
#  error "twi: slave mode not supported by software driver"
# endif
# if AC_TWI_NO_INTERRUPT
#  warning "twi: no interrupt anyway"
# endif

/* Define IO shortcuts. */
# define SDA_IO AC_TWI_SOFT_SDA_IO
# define SCL_IO AC_TWI_SOFT_SCL_IO

# if AC_TWI_PULL_UP
#  define SDA_0 do { IO_CLR (SDA_IO); IO_OUTPUT (SDA_IO); } while (0)
#  define SDA_1 do { IO_INPUT (SDA_IO); IO_SET (SDA_IO); } while (0)
#  define SCL_0 do { IO_CLR (SCL_IO); IO_OUTPUT (SCL_IO); } while (0)
#  define SCL_1 do { IO_INPUT (SCL_IO); IO_SET (SCL_IO); } while (0)
# else
#  define SDA_0 do { IO_OUTPUT (SDA_IO); } while (0)
#  define SDA_1 do { IO_INPUT (SDA_IO); } while (0)
#  define SCL_0 do { IO_OUTPUT (SCL_IO); } while (0)
#  define SCL_1 do { IO_INPUT (SCL_IO); } while (0)
# endif

# if AC_TWI_MASTER_ENABLE

/** Current master status. */
uint8_t twi_master_current_status;

# endif

void
twi_init (uint8_t addr)
{
#if AC_TWI_PULL_UP
    SDA_1;
    SCL_1;
#else
    IO_CLR (SDA_IO);
    SDA_1;
    IO_CLR (SCL_IO);
    SCL_1;
#endif
}

void
twi_uninit (void)
{
    /* Keep pull-up. */
}

# if AC_TWI_MASTER_ENABLE

/** Wait a half bit period. */
static void
twi_delay (void)
{
    utils_delay (0.5 / AC_TWI_FREQ);
}

/** Wait until SCL is high to enable clock stretching. */
static inline void
twi_wait_scl (void)
{
    while (!IO_GET (SCL_IO))
	;
}

/** Send start condition. */
static void
twi_master_send_start (void)
{
    SDA_0;
    twi_delay ();
    SCL_0;
    twi_delay ();
}

/** Send stop condition. */
static void
twi_master_send_stop (void)
{
    SDA_0;
    twi_delay ();
    SCL_1;
    twi_wait_scl ();
    twi_delay ();
    SDA_1;
    twi_delay ();
}

/** Send one bit. */
static void
twi_master_send_bit (uint8_t bit)
{
    if (bit)
	SDA_1;
    else
	SDA_0;
    twi_delay ();
    SCL_1;
    twi_wait_scl ();
    twi_delay ();
    SCL_0;
}

/** Receive one bit. */
static uint8_t
twi_master_recv_bit (void)
{
    SDA_1;
    twi_delay ();
    SCL_1;
    twi_wait_scl ();
    uint8_t bit = IO_GET (SDA_IO) ? 1 : 0;
    twi_delay ();
    SCL_0;
    return bit;
}

/** Send one byte, return the received nack bit. */
static uint8_t
twi_master_send_byte (uint8_t b)
{
    uint8_t i;
    for (i = 8; i; i--)
      {
	twi_master_send_bit (b & 0x80);
	b <<= 1;
      }
    return twi_master_recv_bit ();
}

/** Receive one byte, send the nack bit. */
static uint8_t
twi_master_recv_byte (uint8_t nack)
{
    uint8_t i, b;
    b = 0;
    for (i = 8; i; i--)
      {
	b <<= 1;
	b |= twi_master_recv_bit ();
      }
    twi_master_send_bit (nack);
    return b;
}

void
twi_master_send (uint8_t addr, const uint8_t *buffer, uint8_t size)
{
    uint8_t nack, sent = 0;
    /* Start. */
    twi_master_send_start ();
    /* Send SLA+W. */
    nack = twi_master_send_byte (addr | 0);
    /* Send data. */
    for (; !nack && sent < size; sent++)
	nack = twi_master_send_byte (buffer[sent]);
    /* Stop. */
    twi_master_send_stop ();
    /* Update status, there is no background task. */
    twi_master_current_status = sent;
    /* If defined, call master done callback. */
#ifdef AC_TWI_MASTER_DONE
    AC_TWI_MASTER_DONE ();
#endif
}

void
twi_master_recv (uint8_t addr, uint8_t *buffer, uint8_t size)
{
    uint8_t nack, recv = 0;
    /* Start. */
    twi_master_send_start ();
    /* Send SLA+R. */
    nack = twi_master_send_byte (addr | 1);
    if (!nack)
      {
	/* Receive data, send nack in last byte. */
	for (; recv < size; recv++)
	    buffer[recv] = twi_master_recv_byte (recv == size - 1 ? 1 : 0);
      }
    /* Stop. */
    twi_master_send_stop ();
    /* Update status, there is no background task. */
    twi_master_current_status = recv;
    /* If defined, call master done callback. */
#ifdef AC_TWI_MASTER_DONE
    AC_TWI_MASTER_DONE ();
#endif
}

uint8_t
twi_master_status (void)
{
    return twi_master_current_status;
}

uint8_t
twi_master_wait (void)
{
    /* No background task, nothing to wait. */
    return twi_master_current_status;
}

# endif /* AC_TWI_MASTER_ENABLE */

#endif /* TWI_DRIVER == TWI_DRIVER_SOFT */
