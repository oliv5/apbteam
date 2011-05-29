/* test_servo.c. */
/* avr.servo - Servo AVR module. {{{
 *
 * Copyright (C) 2011 Maxime Hadjinlian
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
#include "modules/uart/uart.h"
#include "modules/proto/proto.h"
#include "modules/devices/servo/servo.h"
#include "modules/utils/utils.h"
#include "modules/utils/byte.h"
#include "io.h"

/* proto_callback is called by proto
 * when a command is complete and valid.
 * proto act as an intelligent buffer for us.
*/
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
      case c ('s', 3):
	/* We are looking to receive a command in the form !sxxyyyy
	 * where xx is the servo id and yyyy is the command itself.
	 * For the size, please read digital/avr/modules/proto/proto.txt
	 */
	servo_set_position (args[0], v8_to_v16 (args[1], args[2]));
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
    servo_init ();

    /* This command should be generaly sent on reset. */
    proto_send0 ('z');
    /* This is to accept commands. */
    while (1)
      {
	uint8_t c = uart0_getc ();
	proto_accept (c);
	/* once the command is complete according to
	 * proto's state machine, proto_callback will
	 * be called with the full command.
	 */
      }
}
