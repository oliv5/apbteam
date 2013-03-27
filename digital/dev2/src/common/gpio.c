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
#include "gpio_proto.h"

#include "descriptors.h"
#include "modules/usb/usb.h"
#include "modules/utils/utils.h"

/* Context. */
struct gpio_t
{
    /* Whether binary mode is activated. */
    uint8_t bin;
    /* Currently handled operation, or 0 if none. */
    uint8_t op;
    /* Arguments being received. */
    uint8_t args[5];
    /* Number of arguments to receive. */
    uint8_t args_nb;
    /* Number of received arguments. */
    uint8_t args_index;
    /* Data was sent on TX endpoint, need ZLP or last packet finalisation. */
    uint8_t data_sent;
};

static struct gpio_t ctx;

void
gpio_init (void)
{
    PORTD = 0;
    DDRD = 0;
    ctx.bin = 0;
}

void
gpio_uninit (void)
{
    gpio_init ();
}

static void
gpio_bin_send (uint8_t c)
{
    Endpoint_SelectEndpoint (GPIO_TX_EPNUM);
    while (!Endpoint_ReadWriteAllowed ())
	;
    Endpoint_Write_Byte (c);
    if (!Endpoint_ReadWriteAllowed ())
	Endpoint_ClearCurrentBank ();
    Endpoint_SelectEndpoint (GPIO_RX_EPNUM);
    ctx.data_sent = 1;
}

static void
gpio_bin_accept (uint8_t c)
{
    if (!ctx.op)
      {
	uint8_t op_args[][2] = {
	      { GPIO_OP_RESET_SYNC, 0 },
	      { GPIO_OP_SETUP, 5 },
	      { GPIO_OP_DIR, 1 },
	      { GPIO_OP_DIR_OUT, 1 },
	      { GPIO_OP_DIR_IN, 1 },
	      { GPIO_OP_OUT, 1 },
	      { GPIO_OP_OUT_SET, 1 },
	      { GPIO_OP_OUT_RESET, 1 },
	      { GPIO_OP_OUT_TOGGLE, 1 },
	      { GPIO_OP_OUT_CHANGE, 2 },
	      { GPIO_OP_IN, 0 },
	};
	uint8_t i;
	ctx.op = 0;
	ctx.args_nb = 0;
	ctx.args_index = 0;
	for (i = 0; i < UTILS_COUNT (op_args); i++)
	  {
	    if (op_args[i][0] == c)
	      {
		ctx.op = c;
		ctx.args_nb = op_args[i][1];
		break;
	      }
	  }
      }
    else
      {
	ctx.args[ctx.args_index++] = c;
      }
    if (ctx.op && ctx.args_index == ctx.args_nb)
      {
	switch (ctx.op)
	  {
	  case GPIO_OP_RESET_SYNC:
	    PORTD = 0;
	    DDRD = 0;
	    break;
	  case GPIO_OP_SETUP:
	    /* Ignore arguments for the moment. */
	    break;
	  case GPIO_OP_DIR:
	    DDRD = ctx.args[0];
	    break;
	  case GPIO_OP_DIR_OUT:
	    DDRD |= ctx.args[0];
	    break;
	  case GPIO_OP_DIR_IN:
	    DDRD &= ~ctx.args[0];
	    break;
	  case GPIO_OP_OUT:
	    PORTD = ctx.args[0];
	    break;
	  case GPIO_OP_OUT_SET:
	    PORTD |= ctx.args[0];
	    break;
	  case GPIO_OP_OUT_RESET:
	    PORTD &= ~ctx.args[0];
	    break;
	  case GPIO_OP_OUT_TOGGLE:
	    PORTD ^= ctx.args[0];
	    break;
	  case GPIO_OP_OUT_CHANGE:
	    PORTD = (PORTD & ~ctx.args[0]) | ctx.args[1];
	    break;
	  case GPIO_OP_IN:
	    gpio_bin_send (PIND);
	    break;
	  }
	ctx.op = 0;
      }
}

void
gpio_task (void)
{
    if (!ctx.bin)
      {
	Endpoint_SelectEndpoint (GPIO_RX_EPNUM);
	/* If data is available from USB: */
	if (Endpoint_ReadWriteAllowed ())
	  {
	    /* Read as much as possible, and clear endpoint. */
	    do {
		uint8_t c = Endpoint_Read_Byte ();
		if (c == GPIO_OP_RESET_SYNC)
		  {
		    ctx.bin = 1;
		    gpio_task ();
		    return;
		  }
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
    else
      {
	Endpoint_SelectEndpoint (GPIO_RX_EPNUM);
	if (Endpoint_ReadWriteAllowed ())
	  {
	    do {
		gpio_bin_accept (Endpoint_Read_Byte ());
	    } while (Endpoint_ReadWriteAllowed ());
	    Endpoint_ClearCurrentBank ();
	  }
	if (ctx.data_sent)
	  {
	    Endpoint_SelectEndpoint (GPIO_TX_EPNUM);
	    Endpoint_ClearCurrentBank ();
	    ctx.data_sent = 0;
	  }
      }
}

