/* serial.c */
/* dev2 - Multi-purpose development board using USB and Ethernet. {{{
 *
 * Copyright (C) 2009 Nicolas Schodet
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

#include "io.h"

#include "serial.h"
#include "descriptors.h"

#include "modules/uart/uart.h"
#include "modules/usb/usb.h"

#define RX D, 2

void
serial_init (void)
{
    /* Use Pull-up on RX pin. */
    IO_DDR (RX) &= ~IO_BV (RX);
    IO_PORT (RX) |= IO_BV (RX);
    uart0_init ();
}

void
serial_uninit (void)
{
    /* Disable UART. */
    UCSR1B = 0;
    IO_PORT (RX) &= ~IO_BV (RX);
}

void
serial_set_params (struct serial_parameters_t *params)
{
    uart0_set_speed (params->speed);
}

void
serial_task (void)
{
    Endpoint_SelectEndpoint (SERIAL_RX_EPNUM);
    /* If data is available from USB: */
    if (Endpoint_ReadWriteAllowed ())
      {
	/* Read as much as possible, and clear endpoint. */
	do {
	    uart0_putc (Endpoint_Read_Byte ());
	} while (Endpoint_ReadWriteAllowed ());
	Endpoint_ClearCurrentBank ();
      }
    /* If data is available from uart and there is room in the TX endpoint: */
    Endpoint_SelectEndpoint (SERIAL_TX_EPNUM);
    if (uart0_poll () && Endpoint_ReadWriteAllowed ())
      {
	do {
	    Endpoint_Write_Byte (uart0_getc ());
	} while (uart0_poll () && Endpoint_ReadWriteAllowed ());
	Endpoint_ClearCurrentBank ();
      }
}

