/* main.c */
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

#include "modules/usb/usb.h"
#include "modules/isp/isp_frame.h"
#include "io.h"

#include <avr/wdt.h>

#include "descriptors.h"
#include "common/serial.h"
#include "common/usb_isp.h"
#include "common/gpio.h"
#include "common/select.h"

HANDLES_EVENT (USB_Connect);
HANDLES_EVENT (USB_Disconnect);
HANDLES_EVENT (USB_ConfigurationChanged);
HANDLES_EVENT (USB_UnhandledControlPacket);

volatile uint8_t usb_connected, usb_configured, gpio_mode;

int
main (void)
{
    /* Disable watchdog if enabled by bootloader/fuses. */
    MCUSR &= ~(1 << WDRF);
    wdt_disable ();
    /* Disable Clock Division. */
    SetSystemClockPrescaler (0);
    /* Initialise hardware. */
    select_init ();
    /* Initialize USB Subsystem. */
    USB_Init ();
    /* Main loop. */
    while (1)
      {
	if (usb_connected)
	    USB_USBTask ();
	if (usb_configured)
	  {
	    serial_task ();
	    if (!gpio_mode)
		usb_isp_task ();
	    else
		gpio_task ();
	  }
      }
}

EVENT_HANDLER (USB_Connect)
{
    usb_connected = 1;
}

EVENT_HANDLER (USB_Disconnect)
{
    usb_connected = 0;
    usb_configured = 0;
}

EVENT_HANDLER (USB_ConfigurationChanged)
{
    /* Setup Rx and Tx Endpoints for the first port. */
    Endpoint_ConfigureEndpoint (SERIAL_TX_EPNUM, EP_TYPE_BULK,
				ENDPOINT_DIR_IN, SERIAL_TX_EPSIZE,
				ENDPOINT_BANK_SINGLE);
    Endpoint_ConfigureEndpoint (SERIAL_RX_EPNUM, EP_TYPE_BULK,
				ENDPOINT_DIR_OUT, SERIAL_RX_EPSIZE,
				ENDPOINT_BANK_SINGLE);
    /* Setup Rx and Tx Endpoints for the second port.
     * Port shared between ISP and GPIO, use control packet to switch. */
    Endpoint_ConfigureEndpoint (ISP_TX_EPNUM, EP_TYPE_BULK,
				ENDPOINT_DIR_IN, ISP_TX_EPSIZE,
				ENDPOINT_BANK_SINGLE);
    Endpoint_ConfigureEndpoint (ISP_RX_EPNUM, EP_TYPE_BULK,
				ENDPOINT_DIR_OUT, ISP_RX_EPSIZE,
				ENDPOINT_BANK_SINGLE);
    /* Start tasks. */
    usb_configured = 1;
    gpio_mode = 0;
}

EVENT_HANDLER (USB_UnhandledControlPacket)
{
    /* Process TS specific control requests. */
    switch (bRequest)
      {
	/* Switch back to DFU mode. */
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
	/* Select output. */
      case 1:
	if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_VENDOR |
			      REQREC_DEVICE))
	  {
	    /* Selector parameter. */
	    uint8_t output = Endpoint_Read_Byte ();
	    Endpoint_ClearSetupReceived ();
	    /* Select output. */
	    serial_uninit ();
	    if (gpio_mode)
		gpio_uninit ();
	    select_out (output);
	    if (select_active (output))
		serial_init ();
	    /* Disable GPIO mode. */
	    gpio_mode = 0;
            /* Send acknowledgement. */
	    Endpoint_ClearSetupIN ();
	  }
	break;
	/* Set serial parameters. */
      case 0x70:
	if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_VENDOR |
			      REQREC_DEVICE))
	  {
	    Endpoint_ClearSetupReceived ();
	    /* Set serial parameters. */
	    struct serial_parameters_t params;
	    Endpoint_Read_Control_Stream_LE (&params, sizeof (params));
	    serial_set_params (&params);
	    /* Send acknowledgement. */
	    Endpoint_ClearSetupIN ();
	  }
	break;
	/* Set GPIO. */
      case 0x80:
	if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_VENDOR |
			      REQREC_DEVICE))
	  {
	    /* GPIO parameter. */
	    uint8_t ddr = Endpoint_Read_Byte ();
	    uint8_t port = Endpoint_Read_Byte ();
	    Endpoint_ClearSetupReceived ();
	    /* Set GPIO. */
	    DDRD = ddr;
	    PORTD = port;
	    /* Also switch to GPIO mode. */
	    gpio_mode = 1;
            /* Send acknowledgement. */
	    Endpoint_ClearSetupIN ();
	  }
	break;
      }
}

