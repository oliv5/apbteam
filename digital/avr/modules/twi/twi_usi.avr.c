/* twi_usi.avr.c - USI implementation. */
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

#if TWI_DRIVER == TWI_DRIVER_USI

# include "io.h"

# include <string.h>

# if AC_TWI_PULL_UP
#  error "twi: pull up not supported by USI driver"
# endif
# if AC_TWI_MASTER_ENABLE
#  error "twi: master mode not supported by USI driver"
# endif

#if defined (__AVR_ATtiny25__) \
    || defined (__AVR_ATtiny45__) \
    || defined (__AVR_ATtiny85__)
# define SDA_IO B, 0
# define SCL_IO B, 2
#else
# error "twi: not defined for this chip"
#endif

#if AC_TWI_NO_INTERRUPT
# define HANDLE_START() static void handle_start (void)
# define HANDLE_OVERFLOW() static void handle_overflow (void)
# define IF_INTR(x) 0
#else
# define HANDLE_START() SIGNAL (USI_START_vect)
# define HANDLE_OVERFLOW() SIGNAL (USI_OVF_vect)
# define IF_INTR(x) (x)
#endif

/** Value to be used for USISR to clear all flags, except start condition
 * which is cleared only if CLEAR_START is 1, and to set the counter so that
 * it overflows after BITS bits tranceived. */
#define USISR_SET(clear_start, bits) \
    (((clear_start) << USISIF) \
     | _BV (USIOIF) | _BV (USIPF) | _BV (USIDC) \
     | (((bits) ? (16 - (bits) * 2) : 0) << USICNT0))

/** Value to be used for USICR when waiting for start condition only. */
#define USICR_START_SET \
    (IF_INTR (_BV (USISIE)) | _BV (USIWM1) | _BV (USICS1))

/** Value to be used for USICR when waiting for start condition or counter
 * overflow. */
#define USICR_START_OR_OVERFLOW_SET \
    (IF_INTR (_BV (USISIE) | _BV (USIOIE)) | _BV (USIWM1) | _BV (USIWM0) \
     | _BV (USICS1))

/** USI slave state, see below for state explanation. */
enum
{
    TWI_USI_WAIT_ADDRESS,
    TWI_USI_RECV,
    TWI_USI_RECV_SEND_ACK,
    TWI_USI_SEND_CHECK_ACK,
    TWI_USI_SEND,
    TWI_USI_SEND_RECV_ACK,
};

/** Module context. */
struct twi_usi_t
{
    /** Slave address. */
    uint8_t address;
    /** Current state, only used on counter overflow. */
    uint8_t state;
    /** Reception buffer. */
    uint8_t recv_buffer[AC_TWI_SLAVE_RECV_BUFFER_SIZE];
    /** Reception buffer current size. */
    uint8_t recv_buffer_size;
    /** Transmission buffer. */
    uint8_t send_buffer[AC_TWI_SLAVE_SEND_BUFFER_SIZE];
    /** Transmission buffer current size. */
    uint8_t send_buffer_size;
    /** Index of last transmitted byte in transmission buffer. */
    uint8_t send_buffer_index;
};

/** Global context. */
static struct twi_usi_t twi_usi_global;
#define ctx twi_usi_global

void
twi_init (uint8_t addr)
{
    /* Reset context. */
    ctx.address = addr;
    ctx.recv_buffer_size = 0;
    ctx.send_buffer_size = 0;
    /* Set IO (SDA input, SCL driven (for clock stretching)), enable USI. */
    IO_SET (SDA_IO);
    IO_SET (SCL_IO);
    USISR = USISR_SET (1, 0);
    USICR = USICR_START_SET;
    IO_INPUT (SDA_IO);
    IO_OUTPUT (SCL_IO);
}

void
twi_uninit (void)
{
    /* Stop USI, set IO to input. */
    IO_INPUT (SDA_IO);
    IO_INPUT (SCL_IO);
    USICR = 0;
    IO_CLR (SDA_IO);
    IO_CLR (SCL_IO);
}

void
twi_slave_update (const uint8_t *buffer, uint8_t size)
{
#if !AC_TWI_NO_INTERRUPT
# warning "twi: driver not safe using interrupts"
#endif
    memcpy (ctx.send_buffer, buffer, size);
    ctx.send_buffer_size = size;
}

/** Handle start condition reception. */
HANDLE_START ()
{
    /* Stop driving SDA (start condition acts as a reset). */
    IO_INPUT (SDA_IO);
    /* Wait until the start condition is finished. */
    while (!IO_GET (SDA_IO) && IO_GET (SCL_IO))
	;
    /* Prepare for address reception. */
    ctx.state = TWI_USI_WAIT_ADDRESS;
    USICR = USICR_START_OR_OVERFLOW_SET;
    USISR = USISR_SET (1, 8);
}

/** Handle USI counter overflow. */
HANDLE_OVERFLOW ()
{
    uint8_t sr, send;
    switch (ctx.state)
      {
      case TWI_USI_WAIT_ADDRESS:
	/* Start condition has been received, check if this is our address,
	 * send ACK and go to receiver or sender state. */
	if ((USIDR & 0xfe) == ctx.address)
	  {
	    send = USIDR & 1;
	    /* Send ACK. */
	    USIDR = 0;
	    IO_OUTPUT (SDA_IO);
	    USISR = USISR_SET (0, 1);
	    /* Next state. */
	    if (send)
	      {
		ctx.send_buffer_index = 0;
		ctx.state = TWI_USI_SEND;
	      }
	    else
	      {
		ctx.recv_buffer_size = 0;
		ctx.state = TWI_USI_RECV;
	      }
	  }
	else
	  {
	    USICR = USICR_START_SET;
	    USISR = USISR_SET (0, 0);
	  }
	break;
    /* Receiver mode. */
      case TWI_USI_RECV:
	/* ACK has been transmitted, receive data.  If we are here, there is
	 * room left to receive this data. */
	IO_INPUT (SDA_IO);
	USISR = USISR_SET (0, 8);
	ctx.state = TWI_USI_RECV_SEND_ACK;
	/* Poll for STOP condition detection.
	 * Here, the master will clock SCL to send data, or will send a stop
	 * condition, wait for one of those events. */
	do
	    sr = USISR;
	while (!(sr & _BV (USIPF))
	       && ((sr >> USICNT0) & 0xf) < 16 - 7 * 2);
	/* On STOP condition stop here. */
	if (sr & _BV (USIPF))
	  {
	    USICR = USICR_START_SET;
	    USISR = USISR_SET (0, 0);
	    AC_TWI_SLAVE_RECV (ctx.recv_buffer, ctx.recv_buffer_size);
	  }
	break;
      case TWI_USI_RECV_SEND_ACK:
	/* Data has been received, send ACK if there is room left. */
	ctx.recv_buffer[ctx.recv_buffer_size++] = USIDR;
	if (ctx.recv_buffer_size < AC_TWI_SLAVE_RECV_BUFFER_SIZE)
	  {
	    USIDR = 0;
	    IO_OUTPUT (SDA_IO);
	    USISR = USISR_SET (0, 1);
	    ctx.state = TWI_USI_RECV;
	  }
	else
	  {
	    USICR = USICR_START_SET;
	    USISR = USISR_SET (0, 0);
	    AC_TWI_SLAVE_RECV (ctx.recv_buffer, ctx.recv_buffer_size);
	  }
	break;
    /* Transmitter mode. */
      case TWI_USI_SEND_CHECK_ACK:
	/* Check received ACK, send if master wants to continue. */
	if (USIDR)
	  {
	    /* NACK received, stop here. */
	    USICR = USICR_START_SET;
	    USISR = USISR_SET (0, 0);
	    break;
	  }
	/* no break */
      case TWI_USI_SEND:
	/* ACK has been transmitted or received, send next data. */
	if (ctx.send_buffer_index < ctx.send_buffer_size)
	  {
	    USIDR = ctx.send_buffer[ctx.send_buffer_index++];
	    IO_OUTPUT (SDA_IO);
	    USISR = USISR_SET (0, 8);
	    ctx.state = TWI_USI_SEND_RECV_ACK;
	  }
	else
	  {
	    /* No data to send, stop here. */
	    IO_INPUT (SDA_IO);
	    USICR = USICR_START_SET;
	    USISR = USISR_SET (0, 0);
	  }
	break;
      case TWI_USI_SEND_RECV_ACK:
	/* Data has been sent, receive ACK. */
	USIDR = 0;
	IO_INPUT (SDA_IO);
	USISR = USISR_SET (0, 1);
	ctx.state = TWI_USI_SEND_CHECK_ACK;
	break;
      }
}

# if AC_TWI_NO_INTERRUPT

void
twi_update (void)
{
    if (USISR & _BV (USISIF))
	handle_start ();
    if (USISR & _BV (USIOIF))
	handle_overflow ();
}

# endif /* AC_TWI_NO_INTERRUPT */

#endif /* TWI_DRIVER == TWI_DRIVER_USI */
