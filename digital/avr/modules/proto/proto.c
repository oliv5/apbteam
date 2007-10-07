/* proto.c */
/* avr.proto - Protocol AVR module. {{{
 *
 * Copyright (C) 2005 Nicolas Schodet
 *
 * Robot APB Team/Efrei 2006.
 *        Web: http://assos.efrei.fr/robot/
 *      Email: robot AT efrei DOT fr
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
#include "proto.h"

#include <ctype.h>

/* +AutoDec */

/** Accept a digit to be used for args. */
static void
proto_accept_digit (uint8_t c);

#if AC_PROTO_QUOTE == 1
/** Accept a quoted char to be used for args. */
static void
proto_accept_char (uint8_t c);
#endif

/* Send a hex digit. */
inline static void
proto_hex (uint8_t h);

/* -AutoDec */

static uint8_t cmd;
static uint8_t size;
static uint8_t args[AC_PROTO_ARGS_MAX_SIZE];

/** Step of decoding:
 *  - 0: nothing received.
 *  - 1: bang received.
 *  - 2: command received.
 *  - 3: command received, and processing a number.
 *  - 4: quote received. 
 *  - 5: double quote received. */
static uint8_t step;

/** Accept a new character. */
void
proto_accept (uint8_t c)
{
    if (c == '!')
	step = 1;
    else
      {
	switch (step)
	  {
	  case 0:
	    /* Nothing received yet. */
	    break;
	  case 1:
	    /* Bang received yet. */
	    if (isalpha (c))
	      {
		cmd = c;
		size = 0;
		step = 2;
	      }
	    else
	      {
		AC_PROTO_CALLBACK ('?', 0, 0);
		step = 0;
	      }
	    break;
	  case 2:
	    /* Command received yet. */
	    if (c == '\r')
	      {
		AC_PROTO_CALLBACK (cmd, size, args);
		step = 0;
	      }
#if AC_PROTO_QUOTE == 1
	    else if (c == '\'')
		step = 4;
	    else if (c == '"')
		step = 5;
#endif
	    else
	      {
		step = 3;
		proto_accept_digit (c);
	      }
	    break;
	  case 3:
	    step--;
	    proto_accept_digit (c);
	    break;
#if AC_PROTO_QUOTE == 1
	  case 4:
	    step = 2;
	    proto_accept_char (c);
	    break;
	  case 5:
	    if (c == '\r')
	      {
		AC_PROTO_CALLBACK (cmd, size, args);
		step = 0;
	      }
	    else
	      {
		proto_accept_char (c);
	      }
	    break;
#endif
	  }
      }
}

/** Accept a digit to be used for args. */
static void
proto_accept_digit (uint8_t c)
{
    /* Test for argument list overflow. */
    if (size >= AC_PROTO_ARGS_MAX_SIZE)
      {
	AC_PROTO_CALLBACK ('?', 0, 0);
	step = 0;
	return;
      }
    /* Convert from hexa. */
    if ('0' <= c && c <= '9')
	c -= '0';
    else if ('a' <= c && c <= 'f')
	c -= 'a' - 10;
    else if ('A' <= c && c <= 'F')
	c -= 'A' - 10;
    else
      {
	AC_PROTO_CALLBACK ('?', 0, 0);
	step = 0;
	return;
      }
    /* Add digit. */
    args[size] <<= 4;
    args[size] |= c;
    if (step == 2)
	size++;
}

#if AC_PROTO_QUOTE == 1
/** Accept a quoted char to be used for args. */
static void
proto_accept_char (uint8_t c)
{
    /* Test for argument list overflow or unwanted char. */
    if (size >= AC_PROTO_ARGS_MAX_SIZE || !isprint (c))
      {
	AC_PROTO_CALLBACK ('?', 0, 0);
	step = 0;
	return;
      }
    /* Add char. */
    args[size] = c;
    size++;
}
#endif

/* Send a hex digit. */
inline static void
proto_hex (uint8_t h)
{
    AC_PROTO_PUTC (h >= 10 ? h - 10 + 'a' : h + '0');
}

/* Send a argument byte. */
void
proto_arg (uint8_t a)
{
    proto_hex ((a >> 4) & 0xf);
    proto_hex ((a >> 0) & 0xf);
}

/** Send a command, generic function. */
void
proto_send (uint8_t cmd, uint8_t size, uint8_t *args)
{
    AC_PROTO_PUTC ('!');
    AC_PROTO_PUTC (cmd);
    while (size--)
	proto_arg (*args++);
    AC_PROTO_PUTC ('\r');
}

