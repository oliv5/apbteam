/* twi_hard.c - Implementation using hardware TWI. */
/* avr.twi - TWI AVR module. {{{
 *
 * Copyright (C) 2005 Clément Demonchy
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

#if TWI_DRIVER == TWI_DRIVER_HARD

# include "modules/utils/utils.h"
# include "io.h"
# include <util/twi.h>
# include <string.h>

# if AC_TWI_MASTER_ENABLE && AC_TWI_SLAVE_ENABLE
/* To support multi-master mode, care should be taken in the master send and
 * receive functions as they can no longer assume that the TWI is idle if
 * master status is busy.
 *
 * In case of arbitration lost and acting as a slave, transfer should be
 * restarted or canceled.  Current code just forget it.
 *
 * When a bus error occurs, the code should be adapted to restart in the right
 * mode. */
#  error "twi: multi-master not implemented"
# endif
# if AC_TWI_PULL_UP
#  error "twi: pull up not supported"
# endif
# if AC_TWI_NO_INTERRUPT
#  warning "twi: no interrupt not supported"
# endif

/* Tested AVR check. */
# if defined (__AVR_ATmega32__)
# elif defined (__AVR_ATmega64__)
# elif defined (__AVR_ATmega128__)
# elif defined (__AVR_ATmega1281__)
# elif defined (__AVR_ATmega164P__)
# elif defined (__AVR_AT90USB646__)
# elif defined (__AVR_AT90USB647__)
# elif defined (__AVR_AT90USB1286__)
# elif defined (__AVR_AT90USB1287__)
# else
#  error "twi: not tested on this chip"
# endif

/* Default TWCR value, activate TWI and TWI interrupts. */
# define TWCR_DEFAULT (_BV (TWEN) | _BV (TWIE))

/* Default TWCR value, plus clear TWINT. */
# define TWCR_OK (_BV (TWINT) | TWCR_DEFAULT)

/* TWBR register value for selected baud rate. */
# define TWBR_VALUE ((AC_FREQ / AC_TWI_FREQ - 16) / 2)
# if TWBR_VALUE > 255
#  error "twi: bad baud rate"
# endif

/* Call master done callback if defined. */
# ifdef AC_TWI_MASTER_DONE
#  define TWI_MASTER_DONE() AC_TWI_MASTER_DONE ()
# else
#  define TWI_MASTER_DONE()
# endif

/** Module context. */
struct twi_hard_t
{
# if AC_TWI_SLAVE_ENABLE
    /** Slave transmission buffer. */
    uint8_t slave_send_buffer[AC_TWI_SLAVE_SEND_BUFFER_SIZE];
    /** Slave transmission buffer size, should never be zero! */
    volatile uint8_t slave_send_buffer_size;
# endif /* AC_TWI_SLAVE_ENABLE */
# if AC_TWI_MASTER_ENABLE
    /** Current master status. */
    volatile uint8_t master_current_status;
    /** Current transfer slave address. */
    volatile uint8_t master_transfer_address;
    /** Current transfer buffer. */
    volatile uint8_t *master_transfer_buffer;
    /** Current transfer buffer size. */
    volatile uint8_t master_transfer_buffer_size;
# endif /* AC_TWI_MASTER_ENABLE */
};

/** Global context. */
static struct twi_hard_t twi_hard_global;
# define ctx twi_hard_global

void
twi_init (uint8_t addr)
{
# if AC_TWI_SLAVE_ENABLE
    /* Initial send buffer, size can not be zero. */
    ctx.slave_send_buffer_size = 1;
    ctx.slave_send_buffer[0] = 0;
    /* Set slave address. */
    TWAR = addr;
# endif /* AC_TWI_SLAVE_ENABLE */
# if AC_TWI_MASTER_ENABLE
    /* No previous transfer. */
    ctx.master_current_status = TWI_MASTER_ERROR;
    /* Set baud rate. */
#  ifdef TWPS0
    TWSR = 0; /* Prescaler. */
#  endif
    TWBR = TWBR_VALUE;
# endif /* AC_TWI_MASTER_ENABLE */
    /* Activate TWI and TWI interrupts. */
# if AC_TWI_SLAVE_ENABLE
    TWCR = TWCR_DEFAULT | _BV (TWEA);
# else
    TWCR = TWCR_DEFAULT;
# endif
}

void
twi_uninit (void)
{
    /* Disable TWI and TWI interrupts. */
    TWCR = 0;
}

# if AC_TWI_SLAVE_ENABLE

void
twi_slave_update (const uint8_t *buffer, uint8_t size)
{
    assert (size && size <= AC_TWI_SLAVE_SEND_BUFFER_SIZE);
    /* Lock interrupts. */
    intr_flags_t flags = intr_lock ();
    /* Copy buffer. */
    memcpy (ctx.slave_send_buffer, buffer, size);
    ctx.slave_send_buffer_size = size;
    /* Unlock. */
    intr_restore (flags);
}

# endif /* AC_TWI_SLAVE_ENABLE */

# if AC_TWI_MASTER_ENABLE

void
twi_master_send (uint8_t addr, const uint8_t *buffer, uint8_t size)
{
    /* Now busy. */
    assert (ctx.master_current_status != TWI_MASTER_BUSY);
    ctx.master_current_status = TWI_MASTER_BUSY;
    /* Record send parameters. */
    ctx.master_transfer_address = addr & 0xfe;
    ctx.master_transfer_buffer_size = size;
    ctx.master_transfer_buffer = (uint8_t *) buffer;
    /* Send start condition. */
    TWCR = TWCR_OK | _BV (TWSTA);
}

void
twi_master_recv (uint8_t addr, uint8_t *buffer, uint8_t size)
{
    /* Now busy. */
    assert (ctx.master_current_status != TWI_MASTER_BUSY);
    ctx.master_current_status = TWI_MASTER_BUSY;
    /* Record receive parameters. */
    ctx.master_transfer_address = addr | 0x01;
    ctx.master_transfer_buffer_size = size;
    ctx.master_transfer_buffer = buffer;
    /* Send start condition. */
    TWCR = TWCR_OK | _BV (TWSTA);
}

uint8_t
twi_master_status (void)
{
    return ctx.master_current_status;
}

uint8_t
twi_master_wait (void)
{
    while (ctx.master_current_status == TWI_MASTER_BUSY)
	;
    return ctx.master_current_status;
}

# endif /* AC_TWI_MASTER_ENABLE */

SIGNAL (TWI_vect)
{
# if AC_TWI_SLAVE_ENABLE
    /** Slave work buffer. */
    static uint8_t slave_buffer[UTILS_MAX (AC_TWI_SLAVE_RECV_BUFFER_SIZE,
					   AC_TWI_SLAVE_SEND_BUFFER_SIZE)];
    /** Slave work buffer size for sending (latched). */
    static uint8_t slave_buffer_size;
# endif /* AC_TWI_SLAVE_ENABLE */
    /** Index in work buffer or master buffer. */
    static uint8_t index;
    /* Handle hardware current state. */
    switch (TW_STATUS)
      {
# if AC_TWI_SLAVE_ENABLE
	/*** Slave transmitter mode. ***/
      case TW_ST_SLA_ACK:
      case TW_ST_ARB_LOST_SLA_ACK:
	/* START + SLA|R + ACK
	 * Start condition for reading detected and acknowledged.
	 * Copy user buffer and send first byte. */
	slave_buffer_size = ctx.slave_send_buffer_size;
	memcpy (slave_buffer, ctx.slave_send_buffer, slave_buffer_size);
	index = 0;
	/* no break; */
      case TW_ST_DATA_ACK:
	/* Previous byte was acknowledged, send next. */
	TWDR = slave_buffer[index++];
	if (index == slave_buffer_size)
	    TWCR = TWCR_OK;
	else
	    TWCR = TWCR_OK | _BV (TWEA);
	break;
      case TW_ST_DATA_NACK:
      case TW_ST_LAST_DATA:
	/* Previous byte was not acknowledged
	 * or previous byte was last byte.
	 * Ready for next transfer. */
	TWCR = TWCR_OK | _BV (TWEA);
	break;
	/*** Slave receiver mode. ***/
      case TW_SR_SLA_ACK:
      case TW_SR_ARB_LOST_SLA_ACK:
      case TW_SR_GCALL_ACK:
      case TW_SR_ARB_LOST_GCALL_ACK:
	/* START + SLA|W + ACK
	 * Start condition for writing detected and acknowledged.
	 * Receive first byte. */
	index = 0;
	if (AC_TWI_SLAVE_RECV_BUFFER_SIZE == 1)
	    TWCR = TWCR_OK;
	else
	    TWCR = TWCR_OK | _BV (TWEA);
	break;
      case TW_SR_DATA_ACK:
      case TW_SR_GCALL_DATA_ACK:
	/* DATA + ACK
	 * Data received and acknowledged, receive next byte. */
	slave_buffer[index++] = TWDR;
	if (index == AC_TWI_SLAVE_RECV_BUFFER_SIZE - 1)
	    TWCR = TWCR_OK;
	else
	    TWCR = TWCR_OK | _BV (TWEA);
	break;
      case TW_SR_DATA_NACK:
      case TW_SR_GCALL_DATA_NACK:
	/* DATA + NACK
	 * Data received, not acknowledged, stop transfer. */
	slave_buffer[index++] = TWDR;
	/* no break; */
      case TW_SR_STOP:
	/* STOP
	 * Stop transfer, ready for next one. */
	TWCR = TWCR_OK | _BV (TWEA);
	/* Call reception callback. */
	AC_TWI_SLAVE_RECV (slave_buffer, index);
	break;
# endif /* AC_TWI_SLAVE_ENABLE */
# if AC_TWI_MASTER_ENABLE
	/*** Master mode. ***/
      case TW_START:
      case TW_REP_START:
	/* Start condition transmitted, send address. */
	TWDR = ctx.master_transfer_address;
	TWCR = TWCR_OK;
	index = 0;
	break;
      case TW_MT_ARB_LOST:
	/* Arbitration lost, same as TW_MR_ARB_LOST.
	 * Retry after running transfer. */
	TWCR = TWCR_OK | _BV (TWSTA);
	break;
      case TW_MT_DATA_ACK:
      case TW_MT_SLA_ACK:
	/* Address or data acknowledged, send more data or stop. */
	if (index < ctx.master_transfer_buffer_size)
	  {
	    TWDR = ctx.master_transfer_buffer[index++];
	    TWCR = TWCR_OK;
	  }
	else
	  {
	    TWCR = TWCR_OK | _BV (TWSTO);
	    ctx.master_current_status = index;
	    /* Call master done callback. */
	    TWI_MASTER_DONE ();
	  }
	break;
      case TW_MR_DATA_NACK:
	/* Data not acknowledged, last byte, stop transfer. */
	ctx.master_transfer_buffer[index++] = TWDR;
	/* no break; */
      case TW_MR_SLA_NACK:
	/* Address not acknowledged, stop. */
      case TW_MT_SLA_NACK:
	/* Address not acknowledged, stop. */
      case TW_MT_DATA_NACK:
	/* Data not acknowledged, there is no more room in slave device,
	 * stop. */
	/* Same code, index is 0 for SLA + NACK. */
	TWCR = TWCR_OK | _BV (TWSTO);
	ctx.master_current_status = index;
	/* Call master done callback. */
	TWI_MASTER_DONE ();
	break;
      case TW_MR_SLA_ACK:
	/* Address acknowledged, receive first data. */
	if (ctx.master_transfer_buffer_size <= 1)
	    TWCR = TWCR_OK;
	else
	    TWCR = TWCR_OK | _BV (TWEA);
	break;
      case TW_MR_DATA_ACK:
	/* Data acknowledged, receive next data. */
	ctx.master_transfer_buffer[index++] = TWDR;
	if (index == ctx.master_transfer_buffer_size - 1)
	    TWCR = TWCR_OK;
	else
	    TWCR = TWCR_OK | _BV (TWEA);
	break;
# endif/* AC_TWI_MASTER_ENABLE */
      case TW_BUS_ERROR:
	/* Bus error condition.
	 * Reset TWI to a sane state. */
	TWCR = TWCR_OK | _BV (TWSTO);
# if AC_TWI_SLAVE_ENABLE
	/* Restore addressable mode. */
	TWCR = TWCR_OK | _BV (TWEA);
# else
	/* Signal error. */
	if (ctx.master_current_status == TWI_MASTER_BUSY)
	  {
	    ctx.master_current_status = TWI_MASTER_ERROR;
	    /* Call master done callback. */
	    TWI_MASTER_DONE ();
	  }
# endif
	break;
      }
}

#endif /* TWI_DRIVER == TWI_DRIVER_HARD */
