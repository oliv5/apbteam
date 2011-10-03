/* test_output.c */
/* motor - Motor control module. {{{
 *
 * Copyright (C) 2011 Nicolas Schodet
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

#include "modules/utils/utils.h"
#include "modules/uart/uart.h"
#include "modules/proto/proto.h"

#include "modules/motor/output/output.h"

#include "io.h"

output_t output[OUTPUT_NB];

int16_t target[OUTPUT_NB];
int16_t current[OUTPUT_NB];

uint16_t output_speed_cpt = 1, output_speed = 250;
uint16_t output_stat_cpt, output_stat;

int
main (void)
{
    uint8_t i;
    for (i = 0; i < OUTPUT_NB; i++)
      {
	output[i].min = 0x10;
	output[i].max = OUTPUT_MAX;
	output_init (i, &output[i]);
      }
    uart0_init ();
    proto_send0 ('z');
    sei ();
    while (1)
      {
	utils_delay_ms (4);
	if (output_speed && !--output_speed_cpt)
	  {
	    for (i = 0; i < OUTPUT_NB; i++)
	      {
		if (target[i] < current[i])
		    current[i]--;
		else if (target[i] > output[i].cur)
		    current[i]++;
		output_set (&output[i], current[i]);
	      }
	    output_speed_cpt = output_speed;
	  }
	if (output_stat && !--output_stat_cpt)
	  {
	    proto_send2w ('W', output[0].cur, output[1].cur);
	    output_stat_cpt = output_stat;
	  }
	while (uart0_poll ())
	    proto_accept (uart0_getc ());
      }
}

/** Handle incoming messages. */
void
proto_callback (uint8_t cmd, uint8_t size, uint8_t *args)
{
    uint8_t i;
    uint8_t ok = 1;
    uint8_t index = args[0];
    int16_t value = v8_to_v16 (args[1], args[2]);
#define c(cmd, size) (cmd << 8 | size)
    switch (c (cmd, size))
      {
      case c ('z', 0):
	/* Reset. */
	utils_reset ();
	break;
      case c ('w', 0):
	/* Set zero value. */
	for (i = 0; i < OUTPUT_NB; i++)
	  {
	    output_set (&output[index], 0);
	    current[index] = target[index] = 0;
	  }
      case c ('w', 3):
	/* Set direct value.
	 * - b: output index.
	 * - w: output value. */
	if (index < OUTPUT_NB)
	  {
	    output_set (&output[index], value);
	    target[index] = current[index];
	  }
	else
	    ok = 0;
	break;
      case c ('t', 3):
	/* Set target.
	 * - b: output index.
	 * - w: output target. */
	if (index < OUTPUT_NB)
	  {
	    current[index] = output[index].cur;
	    target[index] = value;
	  }
	else
	    ok = 0;
	break;
      case c ('s', 2):
	/* Set targeting speed.
	 * - w: loops between update. */
	output_speed_cpt = output_speed = v8_to_v16 (args[0], args[1]);
	break;
      case c ('r', 2):
	/* Set reverse flag.
	 * - b: output index.
	 * - b: reverse flag. */
	if (index < OUTPUT_NB)
	    output_set_reverse (&output[index], args[1]);
	else
	    ok = 0;
	break;
      case c ('W', 2):
	/* Output stats. */
	output_stat_cpt = output_stat = v8_to_v16 (args[0], args[1]);
	break;
      default:
	ok = 0;
	break;
      }
    if (ok)
	proto_send (cmd, size, args);
    else
	proto_send0 ('?');
#undef c
}
