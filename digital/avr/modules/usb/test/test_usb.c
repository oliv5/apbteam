/* test_usb.c */
/* usb - USB device module using LUFA. {{{
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

#include "modules/usb/usb.h"
#include "io.h"

#include <avr/wdt.h>

#include "descriptors.h"

/**
 * Based on DualCDC example from LUFA.  Refers to LUFA Demos for more
 * comments.  There is only 4 endpoints on the at90usb162, therefore a dual
 * CDC-ACM cannot be implemented.  To access this dual "serial port", use a
 * generic usb_serial driver.
 */

HANDLES_EVENT (USB_Connect);
HANDLES_EVENT (USB_Disconnect);
HANDLES_EVENT (USB_ConfigurationChanged);
HANDLES_EVENT (USB_UnhandledControlPacket);

volatile uint8_t USB_USBTask_run;
volatile uint8_t TS1Task_run;
volatile uint8_t TS2Task_run;

void
TS1Task (void);

void
TS2Task (void);

int
main (void)
{
    /* Disable watchdog if enabled by bootloader/fuses. */
    MCUSR &= ~(1 << WDRF);
    wdt_disable ();
    /* Disable Clock Division. */
    SetSystemClockPrescaler (0);
    /* Initialize USB Subsystem. */
    USB_Init ();
    /* Main loop. */
    while (1)
      {
	if (USB_USBTask_run)
	    USB_USBTask ();
	if (TS1Task_run)
	    TS1Task ();
	if (TS2Task_run)
	    TS2Task ();
      }
}

EVENT_HANDLER (USB_Connect)
{
    /* Start USB management task. */
    USB_USBTask_run = 1;
}

EVENT_HANDLER (USB_Disconnect)
{
    USB_USBTask_run = 0;
    TS1Task_run = 0;
    TS2Task_run = 0;
}

EVENT_HANDLER (USB_ConfigurationChanged)
{
    /* Setup Rx and Tx Endpoints for the first port. */
    Endpoint_ConfigureEndpoint (TS1_TX_EPNUM, EP_TYPE_BULK,
				ENDPOINT_DIR_IN, TS_TXRX_EPSIZE,
				ENDPOINT_BANK_SINGLE);
    Endpoint_ConfigureEndpoint (TS1_RX_EPNUM, EP_TYPE_BULK,
				ENDPOINT_DIR_OUT, TS_TXRX_EPSIZE,
				ENDPOINT_BANK_SINGLE);
    /* Setup Rx and Tx Endpoints for the second port. */
    Endpoint_ConfigureEndpoint (TS2_TX_EPNUM, EP_TYPE_BULK,
				ENDPOINT_DIR_IN, TS_TXRX_EPSIZE,
				ENDPOINT_BANK_SINGLE);
    Endpoint_ConfigureEndpoint (TS2_RX_EPNUM, EP_TYPE_BULK,
				ENDPOINT_DIR_OUT, TS_TXRX_EPSIZE,
				ENDPOINT_BANK_SINGLE);
    /* Start tasks. */
    TS1Task_run = 1;
    TS2Task_run = 1;
}

EVENT_HANDLER (USB_UnhandledControlPacket)
{
    /* Discard the unused wValue parameter. */
    Endpoint_Ignore_Word ();
    /* wIndex indicates the interface being controlled. */
    Endpoint_Ignore_Word ();
    /* Process TS specific control requests. */
    switch (bRequest)
      {
      case 0:
	if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_VENDOR |
			      REQREC_DEVICE))
	  {
	    Endpoint_ClearSetupReceived ();
            /* Send acknowledgement and wait for it to be sent. */
	    Endpoint_ClearSetupIN ();
	    while (!Endpoint_IsSetupINReady ())
		;
	    USB_ShutDown ();
            /* Jump to bootloader. */
	    ((void (*) (void)) 0x3000) ();
	  }
	break;
      }
}

void
TS1Task (void)
{
    /* Select the Serial Rx Endpoint. */
    Endpoint_SelectEndpoint (TS1_RX_EPNUM);
    /* Check to see if any data has been received. */
    if (Endpoint_ReadWriteAllowed ())
      {
	/* Create a temp buffer big enough to hold the incoming endpoint
	 * packet. */
	uint8_t Buffer[Endpoint_BytesInEndpoint ()];
	/* Remember how large the incoming packet is. */
	uint16_t DataLength = Endpoint_BytesInEndpoint ();
	/* Read in the incoming packet into the buffer. */
	Endpoint_Read_Stream_LE (&Buffer, DataLength);
	/* Finalize the stream transfer to send the last packet. */
	Endpoint_ClearCurrentBank ();
	/* Select the Serial Tx Endpoint. */
	Endpoint_SelectEndpoint (TS1_TX_EPNUM);
	/* Write the received data to the endpoint. */
	Endpoint_Write_Stream_LE (&Buffer, DataLength);
	/* Finalize the stream transfer to send the last packet. */
	Endpoint_ClearCurrentBank ();
      }
}

void
TS2Task (void)
{
    /* Select the Serial Rx Endpoint. */
    Endpoint_SelectEndpoint (TS2_RX_EPNUM);
    /* Check to see if any data has been received. */
    if (Endpoint_ReadWriteAllowed ())
      {
	uint16_t i;
	/* Create a temp buffer big enough to hold the incoming endpoint
	 * packet. */
	uint8_t Buffer[Endpoint_BytesInEndpoint ()];
	/* Remember how large the incoming packet is. */
	uint16_t DataLength = Endpoint_BytesInEndpoint ();
	/* Read in the incoming packet into the buffer. */
	Endpoint_Read_Stream_LE (&Buffer, DataLength);
	/* Finalize the stream transfer to send the last packet. */
	Endpoint_ClearCurrentBank ();
        /* Convert to upper case. */
        for (i = 0; i < DataLength; i++)
            if (Buffer[i] >= 'a' && Buffer[i] <= 'z')
                Buffer[i] -= 'a' - 'A';
	/* Select the Serial Tx Endpoint. */
	Endpoint_SelectEndpoint (TS2_TX_EPNUM);
	/* Write the received data to the endpoint. */
	Endpoint_Write_Stream_LE (&Buffer, DataLength);
	/* Finalize the stream transfer to send the last packet. */
	Endpoint_ClearCurrentBank ();
      }
}

