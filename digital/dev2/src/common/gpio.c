/* gpio.c */
/* dev2 - Multi-purpose development board using USB and Ethernet. {{{
 *
 * Copyright (C) 2013 Nicolas Schodet
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

#include "gpio.h"

#include "descriptors.h"
#include "modules/usb/usb.h"

void
gpio_init (void)
{
    PORTD = 0;
    DDRD = 0;
}

void
gpio_uninit (void)
{
    gpio_init ();
}

void
gpio_task (void)
{
    Endpoint_SelectEndpoint (GPIO_RX_EPNUM);
    /* If data is available from USB: */
    if (Endpoint_ReadWriteAllowed ())
      {
	/* Read as much as possible, and clear endpoint. */
	do {
	    Endpoint_Discard_Byte ();
	} while (Endpoint_ReadWriteAllowed ());
	Endpoint_ClearCurrentBank ();
	/* Now, print current GPIO state if possible. */
	Endpoint_SelectEndpoint (GPIO_TX_EPNUM);
	if (Endpoint_ReadWriteAllowed ())
	  {
	    uint8_t i, pin;
	    pin = PIND;
	    for (i = 0; i < 8; i++)
	      {
		Endpoint_Write_Byte (pin & 0x80 ? '1' : '0');
		pin <<= 1;
	      }
	    Endpoint_Write_Byte ('\r');
	    Endpoint_ClearCurrentBank ();
	  }
      }
}

