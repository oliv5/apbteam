/* test_encoder.c */
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
#include "io.h"
#include "modules/utils/utils.h"
#include "modules/uart/uart.h"
#include "modules/proto/proto.h"

#include "modules/motor/encoder/encoder.h"
#include "modules/motor/encoder/encoder_corrector.h"

uint8_t read, read_cpt, read_mode;
uint8_t ind, ind_cpt, ind_init;
uint8_t count, count_cpt;

encoder_t encoder[AC_ENCODER_EXT_NB];
encoder_corrector_t encoder_corrector_right;

#define TIMER_TOP 255
#define TIMER_STEPS 4
#define TIMER_STEP ((TIMER_TOP + 1) / TIMER_STEPS)

void
timer_init (void)
{
#ifndef HOST
    TCCR0 = regv (FOC0, WGM00, COM01, COM0, WGM01, CS02, CS01, CS00,
		     0,     0,     0,    0,     0,    1,    1,    0);
#endif
}

void
timer_wait (void)
{
#ifndef HOST
    uint8_t i;
    /* Make small steps with counter updates. */
    for (i = 1; i < TIMER_STEPS; i++)
      {
	while (TCNT0 < i * TIMER_STEP)
	    ;
	if (count)
	    encoder_update_step ();
      }
    /* Wait overflow. */
    while (!(TIFR & _BV (TOV0)))
	;
    /* Write 1 to clear. */
    TIFR = _BV (TOV0);
#else
    if (count)
	encoder_update_step ();
#endif
}

int
main (void)
{
    uint8_t i;
#ifndef HOST
    uint8_t read_old = 0;
    uint8_t old_ind = 0;
    const int total = 5000;
#endif
    timer_init ();
    for (i = 0; i < AC_ENCODER_EXT_NB; i++)
	encoder_init (i, &encoder[i]);
    encoder_corrector_init (&encoder_corrector_right);
    uart0_init ();
    proto_send0 ('z');
    sei ();
    while (1)
      {
	timer_wait ();
	if (count)
	  {
	    encoder_update ();
	    encoder_corrector_update (&encoder_corrector_right, &encoder[1]);
	  }
#ifndef HOST
	if (read && !--read_cpt)
	  {
	    uint8_t r0, r1, r2, r3;
	    r0 = encoder_ext_read (0);
	    r1 = encoder_ext_read (1);
	    r2 = encoder_ext_read (2);
	    r3 = encoder_ext_read (3);
	    if (read_mode == 0 || (read_mode == 1 && r3 != read_old)
		|| (read_mode == 2
		    && (r0 == 0 || r1 == 0 || r2 == 0 || r3 == 0)))
	      {
		proto_send4b ('r', r0, r1, r2, r3);
		read_old = r3;
	      }
	    read_cpt = read;
	  }
	if (ind && !--ind_cpt)
	  {
	    i = encoder_ext_read (3);
	    if (!ind_init && i != old_ind)
	      {
		uint8_t eip = old_ind + total;
		uint8_t eim = old_ind - total;
		proto_send7b ('i', old_ind, i, eip, eim, i - eip, i - eim,
			      i == eip || i == eim);
	      }
	    old_ind = i;
	    ind_init = 0;
	    ind_cpt = ind;
	  }
#endif
	if (count && !--count_cpt)
	  {
	    proto_send4w ('C', encoder[0].cur, encoder[1].cur,
			  encoder[2].cur, encoder[3].cur);
	    count_cpt = count;
	  }
	while (uart0_poll ())
	    proto_accept (uart0_getc ());
      }
}

/** Handle incoming messages. */
void
proto_callback (uint8_t cmd, uint8_t size, uint8_t *args)
{
#define c(cmd, size) (cmd << 8 | size)
    switch (c (cmd, size))
      {
      case c ('z', 0):
	/* Reset. */
	utils_reset ();
	break;
      case c ('r', 1):
	/* Output encoders raw value after every read. */
	read_cpt = read = args[0];
	read_mode = 0;
	break;
      case c ('R', 1):
	/* Output encoders raw value only if last encoder changed. */
	read_cpt = read = args[0];
	read_mode = 1;
	break;
      case c ('Z', 1):
	/* Output encoders raw value if any encoder is null. */
	read_cpt = read = args[0];
	read_mode = 2;
	break;
      case c ('i', 1):
	/* Index checking mode.  Require counter_index_test. */
	ind_cpt = ind = args[0];
	ind_init = 1;
	break;
      case c ('C', 1):
	/* Regular encoder output, use encoder module code. */
	count_cpt = count = args[0];
	break;
      case c ('c', 4):
	/* Set correction. */
	encoder_corrector_set_correction (&encoder_corrector_right,
					  v8_to_v32 (args[0], args[1],
						     args[2], args[3]));
	break;
      default:
	proto_send0 ('?');
	return;
      }
    proto_send (cmd, size, args);
#undef c
}
