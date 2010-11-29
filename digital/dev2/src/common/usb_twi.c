/* usb_twi.c */
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
#include "common.h"

#include "usb_twi.h"
#include "descriptors.h"

#include "modules/twi/twi.h"
#include "modules/usb/usb.h"

/** Parser steps. */
enum
{
    /** Nothing received yet. */
    PARSER_WAIT_COMMAND,
    /** Command received, wait address most significant quartet. */
    PARSER_WAIT_ADDRESS_MSQ,
    /** First address quartet received, wait second one. */
    PARSER_WAIT_ADDRESS_LSQ,
    /** Wait data or end of line. */
    PARSER_WAIT_DATA_OR_EOL,
    /** Wait data second quartet. */
    PARSER_WAIT_DATA_LSQ,
};

/** Context. */
struct usb_twi_t
{
    /** Buffer used for transmission or reception. */
    uint8_t buffer[TWI_BUFFER_SIZE];
    /** Number of bytes in buffer. */
    uint8_t buffer_tail;
    /** Number of bytes sent back to USB. */
    uint8_t buffer_head;
    /** Command to execute. */
    char command;
    /** Slave address. */
    uint8_t slave;
    /** Parser step. */
    uint8_t parser_step;
    /** Parser first quartet. */
    uint8_t parser_msq;
    /** Output buffer (in case of USB not ready). */
    char output_buffer[4];
    /** Position in output buffer of next character to send. */
    uint8_t output_buffer_head;
};

/** Global context. */
struct usb_twi_t usb_twi_global;
#define ctx usb_twi_global

void
usb_twi_init (void)
{
    twi_init (0xfe);
}

void
usb_twi_uninit (void)
{
    twi_uninit ();
}

/** Return non zero if there is characters to send. */
static uint8_t
usb_twi_poll (void)
{
    return ctx.output_buffer[ctx.output_buffer_head];
}

/** Output byte to output buffer. */
static void
usb_twi_ouput_byte (uint8_t p, uint8_t b)
{
    uint8_t i;
    char c;
    for (i = 0; i < 2; i++)
      {
	c = "0123456789abcdef"[b >> 4];
	ctx.output_buffer[p++] = c;
	b <<= 4;
      }
}

/** Return a character from the output buffer and reload it from buffer if
 * needed. */
static char
usb_twi_getc (void)
{
    char c;
    uint8_t p;
    /* Take a character in output buffer. */
    c = ctx.output_buffer[ctx.output_buffer_head];
    ctx.output_buffer_head++;
    /* Reload from buffer if needed. */
    if (!ctx.output_buffer[ctx.output_buffer_head]
	&& ctx.buffer_head != ctx.buffer_tail)
      {
	ctx.output_buffer_head = 0;
	p = 0;
	usb_twi_ouput_byte (p, ctx.buffer[ctx.buffer_head++]);
	p += 2;
	if (ctx.buffer_head == ctx.buffer_tail)
	    ctx.output_buffer[p++] = '\r';
	ctx.output_buffer[p] = '\0';
      }
    return c;
}

static uint8_t
usb_twi_get_hex (char c)
{
    if (c >= '0' && c <= '9')
	return c - '0';
    else if (c >= 'a' && c <= 'f')
	return c - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
	return c - 'A' + 10;
    else
	return 0xff;
}

static uint8_t
usb_twi_exec (void)
{
    uint8_t r, p, l;
    if (ctx.command == 's')
      {
	twi_master_send (ctx.slave, ctx.buffer, ctx.buffer_tail);
	r = twi_master_wait ();
	ctx.buffer[0] = r;
	ctx.buffer_head = 0;
	ctx.buffer_tail = 1;
	ctx.output_buffer_head = 0;
	p = 0;
	ctx.output_buffer[p++] = 'S';
	ctx.output_buffer[p++] = '\0';
	return 1;
      }
    else if (ctx.command == 'r' && ctx.buffer_tail == 1)
      {
	l = ctx.buffer[0];
	twi_master_recv (ctx.slave, ctx.buffer, l);
	r = twi_master_wait ();
	ctx.buffer_head = 0;
	ctx.buffer_tail = r;
	ctx.output_buffer_head = 0;
	p = 0;
	ctx.output_buffer[p++] = 'R';
	if (r == 0)
	    ctx.output_buffer[p++] = '\r';
	ctx.output_buffer[p++] = '\0';
	return 1;
      }
    else
	return 0;
}

static void
usb_twi_accept (char c)
{
    uint8_t q;
    /* Purge output buffer. */
    ctx.output_buffer_head = 0;
    ctx.output_buffer[0] = '\0';
    /* Accept FSM. */
    switch (ctx.parser_step)
      {
      case PARSER_WAIT_COMMAND:
	if (c == 's' || c == 'r')
	  {
	    ctx.command = c;
	    ctx.buffer_tail = 0;
	    ctx.parser_step++;
	    return;
	  }
	break;
      case PARSER_WAIT_ADDRESS_MSQ:
	q = usb_twi_get_hex (c);
	if (q != 0xff)
	  {
	    ctx.parser_msq = q;
	    ctx.parser_step++;
	    return;
	  }
	break;
      case PARSER_WAIT_ADDRESS_LSQ:
	q = usb_twi_get_hex (c);
	if (q != 0xff)
	  {
	    ctx.slave = ctx.parser_msq << 4 | q;
	    ctx.parser_step++;
	    return;
	  }
	break;
      case PARSER_WAIT_DATA_OR_EOL:
	if (c == '\r')
	  {
	    ctx.parser_step = PARSER_WAIT_COMMAND;
	    if (usb_twi_exec ())
		return;
	  }
	else if (ctx.buffer_tail < TWI_BUFFER_SIZE)
	  {
	    q = usb_twi_get_hex (c);
	    if (q != 0xff)
	      {
		ctx.parser_msq = q;
		ctx.parser_step++;
		return;
	      }
	  }
	break;
      case PARSER_WAIT_DATA_LSQ:
	q = usb_twi_get_hex (c);
	if (q != 0xff)
	  {
	    ctx.buffer[ctx.buffer_tail++] = ctx.parser_msq << 4 | q;
	    ctx.parser_step--;
	    return;
	  }
	break;
      }
    /* If here, there was an error. */
    ctx.buffer_tail = ctx.buffer_head = 0;
    ctx.output_buffer_head = 0;
    ctx.output_buffer[0] = '!';
    ctx.output_buffer[1] = '\r';
    ctx.output_buffer[2] = '\0';
    ctx.parser_step = PARSER_WAIT_COMMAND;
}

void
usb_twi_task (void)
{
    Endpoint_SelectEndpoint (TWI_RX_EPNUM);
    /* If data is available from USB: */
    if (Endpoint_ReadWriteAllowed ())
      {
	/* Read as much as possible, and clear endpoint. */
	do {
	    usb_twi_accept (Endpoint_Read_Byte ());
	} while (Endpoint_ReadWriteAllowed ());
	Endpoint_ClearCurrentBank ();
      }
    /* If data is available from uart and there is room in the TX endpoint: */
    Endpoint_SelectEndpoint (TWI_TX_EPNUM);
    if (usb_twi_poll () && Endpoint_ReadWriteAllowed ())
      {
	do {
	    Endpoint_Write_Byte (usb_twi_getc ());
	} while (usb_twi_poll () && Endpoint_ReadWriteAllowed ());
	Endpoint_ClearCurrentBank ();
      }
}

