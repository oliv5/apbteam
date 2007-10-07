/* test_proto.c */
/*  {{{
 *
 * Copyright (C) 2004 Nicolas Schodet
 *
 * Robot APB Team/Efrei 2005.
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
#include "io.h"
#include "modules/uart/uart.h"
#include "modules/proto/proto.h"
#include "modules/utils/utils.h"

void
proto_callback (uint8_t cmd, uint8_t size, uint8_t *args)
{
/* This macro combine command and size in one integer. */
#define c(cmd, size) (cmd << 8 | size)
    switch (c (cmd, size))
      {
      case c ('z', 0):
	/* This should be generaly implemented. */
	utils_reset ();
	break;
      case c ('a', 1):
      case c ('a', 4):
      case c ('b', 2):
      case c ('c', 3):
      case c ('d', 4):
      case c ('e', 5):
      case c ('f', 6):
      case c ('g', 7):
      case c ('h', 8):
	/* Accepts some command, and test some commands. */
	proto_send1b ('e', 0xf0);
	proto_send2w ('l', 0x1234, 0x5678);
	proto_send1d ('o', 0x12345678);
	  {
	    volatile uint32_t i = 0x12345678;
	    proto_send1d ('i', i);
	  }
	break;
      default:
	/* This is to handle default commands, return an error. */
	proto_send0 ('?');
	return;
      }
    /* When no error acknoledge. */
    proto_send (cmd, size, args);
#undef c
}

int
main (int argc, char **argv)
{
    avr_init (argc, argv);
    sei ();
    uart0_init ();
    /* This command should be generaly sent on reset. */
    proto_send0 ('z');
    /* This is to accept commands. */
    while (1)
      {
	uint8_t c = uart0_getc ();
	proto_accept (c);
      }
}
