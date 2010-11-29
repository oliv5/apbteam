#ifndef twi_h
#define twi_h
/* twi.h */
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

#if !AC_TWI_SLAVE_ENABLE && !AC_TWI_MASTER_ENABLE
# error "twi: no enabled mode"
#endif

/** Initialise module, configure slave address.  Slave address is ignored if
 * slave mode is disabled. */
void
twi_init (uint8_t addr);

/** Uninitialise module, will no longer respond to bus events. */
void
twi_uninit (void);

#if AC_TWI_SLAVE_ENABLE

# if AC_TWI_SLAVE_POLLED
#  ifdef AC_TWI_SLAVE_RECV
#   error "twi: AC_TWI_SLAVE_RECV should not be set in polled mode"
#  endif
#  define AC_TWI_SLAVE_RECV twi_slave_recv_polled
# endif

/** Called on data reception from master.
 *
 * May run under interrupt context.  Bus may be held until this function
 * returns, so it has to operate quickly. */
void
AC_TWI_SLAVE_RECV (const uint8_t *buffer, uint8_t size);

/** Update buffer to be sent to master if requested. */
void
twi_slave_update (const uint8_t *buffer, uint8_t size);

# if AC_TWI_SLAVE_POLLED

/** If new data has been received, copy received data to provided buffer and
 * return received data size.  Else, return 0. */
uint8_t
twi_slave_poll (uint8_t *buffer, uint8_t size);

# endif

#endif /* AC_TWI_SLAVE_ENABLE */

#if AC_TWI_MASTER_ENABLE

/** Send data to a slave.  This is non blocking, you have to poll master
 * status or use a callback to determine if transfer is terminated.
 *
 * Input buffer is not copied, it should be kept valid until transmission is
 * finished. */
void
twi_master_send (uint8_t addr, const uint8_t *buffer, uint8_t size);

/** Receive data from a slave.  This is non blocking, you have to poll master
 * status or use a callback to determine if transfer is terminated.
 *
 * Input buffer is directly written, and should be kept valid until reception
 * is finished. */
void
twi_master_recv (uint8_t addr, uint8_t *buffer, uint8_t size);

/** Master status, any other values are the number of bytes transferred. */
enum
{
    /** Busy transferring data. */
    TWI_MASTER_BUSY = 0xff,
    /** Last transfer finished badly, ready to process new transfer.
     * Among errors: no acknowledge, bad CRC (if activated), arbitration
     * lost... */
    TWI_MASTER_ERROR = 0,
};

/** Get current master status. */
uint8_t
twi_master_status (void);

/** Wait current master transfer completion and return new status. */
uint8_t
twi_master_wait (void);

# ifdef AC_TWI_MASTER_DONE

/** Called on master transfer completion.
 *
 * May run under interrupt context. */
void
AC_TWI_MASTER_DONE (void);

# endif

#endif /* AC_TWI_MASTER_ENABLE */

#if AC_TWI_NO_INTERRUPT

/** To be call on regular basis, poll events to simulate interrupts. */
void
twi_update (void);

#endif /* AC_TWI_NO_INTERRUPT */

/* Define selected driver. */
#define TWI_DRIVER_HARD 'h'
#define TWI_DRIVER_SOFT 's'
#define TWI_DRIVER_USI 'u'
#define TWI_DRIVER__(drv) TWI_DRIVER_ ## drv
#define TWI_DRIVER_(drv) TWI_DRIVER__ (drv)
#define TWI_DRIVER TWI_DRIVER_ (AC_TWI_DRIVER)

#endif /* twi_h */
