/* usb_isp.c */
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

#include "usb_isp.h"
#include "descriptors.h"
#include "serial.h"

#include "modules/isp/isp.h"
#include "modules/isp/isp_frame.h"
#include "modules/isp/isp_proto.h"
#include "modules/spi/spi.h"
#include "modules/usb/usb.h"
#include "modules/utils/utils.h"

#define RESET D, 4
#define SCK AC_SPI0_SOFT_SCK_IO

static uint8_t usb_isp_sent;

void
usb_isp_send_char (uint8_t c)
{
    Endpoint_SelectEndpoint (ISP_TX_EPNUM);
    /* Wait endpoint to become ready. */
    while (!Endpoint_ReadWriteAllowed ())
	;
    Endpoint_Write_Byte (c);
    /* If at end of endpoint buffer, send. */
    if (!Endpoint_ReadWriteAllowed ())
	Endpoint_ClearCurrentBank ();
    /* Select back RX endpoint. */
    Endpoint_SelectEndpoint (ISP_RX_EPNUM);
    /* Will need extra clear at end of all transfers. */
    usb_isp_sent = 1;
}

void
usb_isp_spi_enable (void)
{
    /* May have to disable serial. */
    if (AC_DEV2_SERIAL_ISP_SHARED)
	serial_uninit ();
    /* Reset slave AVR. */
    IO_DDR (RESET) |= IO_BV (RESET);
    /* Set SCK to low. */
    IO_DDR (SCK) |= IO_BV (SCK);
    /* Reset pulse, at least 2 clock cycle later, be conservative. */
    utils_delay_ms (1);
    IO_PORT (RESET) |= IO_BV (RESET);
    IO_PORT (RESET) &= ~IO_BV (RESET);
    /* Enable SPI. */
    spi_init (SPI_MASTER, SPI_MODE_0, SPI_MSB_FIRST, isp_proto_sck_duration);
}

void
usb_isp_spi_disable (void)
{
    /* Disable SPI. */
    spi_uninit ();
    /* Release reset, SCK is handled by SPI driver. */
    IO_DDR (RESET) &= ~IO_BV (RESET);
    /* May have to enable serial. */
    if (AC_DEV2_SERIAL_ISP_SHARED)
	serial_init ();
}

void
usb_isp_spi_sck_pulse (void)
{
    IO_PORT (SCK) ^= IO_BV (SCK);
    IO_PORT (SCK) ^= IO_BV (SCK);
}

uint8_t
usb_isp_spi_tx (uint8_t data)
{
    return spi_send_and_recv (data);
}

void
usb_isp_task (void)
{
    Endpoint_SelectEndpoint (ISP_RX_EPNUM);
    /* If data is available from USB: */
    if (Endpoint_ReadWriteAllowed ())
      {
	/* Read as much as possible, and clear endpoint. */
	do {
	    isp_frame_accept_char (Endpoint_Read_Byte ());
	} while (Endpoint_ReadWriteAllowed ());
	Endpoint_ClearCurrentBank ();
      }
    /* If data has been sent, sent a ZLP or finalise last packet. */
    if (usb_isp_sent)
      {
	Endpoint_SelectEndpoint (ISP_TX_EPNUM);
	Endpoint_ClearCurrentBank ();
	usb_isp_sent = 0;
      }
}

