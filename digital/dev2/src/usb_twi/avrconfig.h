#ifndef avrconfig_h
#define avrconfig_h
/* avrconfig.h */
/* dev2 - Multi-purpose development board using USB and Ethernet. {{{
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

/* utils */
/** AVR Frequency : 1000000, 1843200, 2000000, 3686400, 4000000, 7372800,
 * 8000000, 11059200, 14745600, 16000000, 18432000, 20000000. */
#error "please check frequency"
#define AC_FREQ 8000000

/* twi - TWI module. */
/** Driver to implement TWI: HARD, SOFT, or USI. */
#define AC_TWI_DRIVER SOFT
/** Do not use interrupts. */
#define AC_TWI_NO_INTERRUPT 0
/** TWI frequency, should really be 100 kHz. */
#define AC_TWI_FREQ 10000
/** Enable slave part. */
#define AC_TWI_SLAVE_ENABLE 0
/** Enable master part. */
#define AC_TWI_MASTER_ENABLE 1
/** Use polled slave mode: received data is stored in a buffer which can be
 * polled using twi_slave_poll. */
#define AC_TWI_SLAVE_POLLED 1
/** Slave reception callback to be defined by the user when not in polled
 * mode. */
#undef AC_TWI_SLAVE_RECV
/** Master transfer completion callback, optionally defined by the user, called
 * at end of master transfer. */
#undef AC_TWI_MASTER_DONE
/** Use internal pull up. */
#define AC_TWI_PULL_UP 1
/** SDA line IO for SOFT driver. */
#define AC_TWI_SOFT_SDA_IO D, 7
/** SCL line IO for SOFT driver. */
#define AC_TWI_SOFT_SCL_IO D, 5

/* usb */
#include "modules/usb/lufaconfig.h"

#endif /* avrconfig_h */
