/* main.c */
/* robospierre - Eurobot 2011 AI. {{{
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

#include "timer.h"
#include "chrono.h"
#include "simu.host.h"

#include "asserv.h"
#include "mimot.h"
#include "twi_master.h"

#include "pwm.h"
#include "contact.h"

#include "io.h"

/** Our color. */
enum team_color_e team_color;

/** Asserv stats counters. */
static uint8_t main_stats_asserv_, main_stats_asserv_cpt_;

/** Contact stats counters. */
static uint8_t main_stats_contact_, main_stats_contact_cpt_;

/** Main initialisation. */
static void
main_init (void)
{
    /* Serial port */
    uart0_init ();
    /* Enable interrupts */
    sei ();
    /* Main timer */
    timer_init ();
    /* TWI communications */
    asserv_init ();
    mimot_init ();
    twi_master_init ();
    /* IO modules. */
    pwm_init ();
    contact_init ();
    /* Initialization done. */
    proto_send0 ('z');
}

/** Main events management. */
void
main_event_to_fsm (void)
{
}

/** Main (and infinite) loop. */
static void
main_loop (void)
{
    while (1)
      {
	/* Wait until next cycle. */
	timer_wait ();
	/* Update chrono. */
	chrono_update ();
	/* Is match over? */
	if (chrono_is_match_over ())
	  {
	    /* End it and block here indefinitely. */
	    chrono_end_match (42);
	    return;
	  }
	/* Handle commands from UART. */
	while (uart0_poll ())
	    proto_accept (uart0_getc ());
	/* Update IO modules. */
	pwm_update ();
	contact_update ();
	/* Only manage events if slaves are synchronised. */
	if (twi_master_sync ())
	    main_event_to_fsm ();
	/* Send stats if requested. */
	if (main_stats_asserv_ && !--main_stats_asserv_cpt_)
	  {
	    /* Get current position */
	    position_t cur_pos;
	    asserv_get_position (&cur_pos);
	    /* Send stats */
	    proto_send3w ('A', cur_pos.v.x, cur_pos.v.y, cur_pos.a);
	    /* Reset stats counter */
	    main_stats_asserv_cpt_ = main_stats_asserv_;
	  }
	if (main_stats_contact_ && !--main_stats_contact_cpt_)
	  {
	    proto_send1d ('P', contact_all ());
	    main_stats_contact_cpt_ = main_stats_contact_;
	  }
      }
}

/** Handle received commands. */
void
proto_callback (uint8_t cmd, uint8_t size, uint8_t *args)
{
#define c(cmd, size) (cmd << 8 | size)
    switch (c (cmd, size))
      {
      case c ('z', 0):
	/* Reset */
	utils_reset ();
	break;
      case c ('w', 3):
	/* Set PWM.
	 * - 1b: index.
	 * - 1w: value. */
	pwm_set (args[0], v8_to_v16 (args[1], args[2]));
	break;
      case c ('w', 7):
	/* Set timed PWM.
	 * - 1b: index.
	 * - 1w: value.
	 * - 1w: time.
	 * - 1w: rest value. */
	pwm_set_timed (args[0], v8_to_v16 (args[1], args[2]),
		       v8_to_v16 (args[3], args[4]),
		       v8_to_v16 (args[5], args[6]));
	break;
	/* Stats commands.
	 * - b: interval between stats. */
      case c ('A', 1):
	/* Position stats. */
	main_stats_asserv_ = main_stats_asserv_cpt_ = args[0];
	break;
      case c ('P', 1):
	/* Contact stats. */
	main_stats_contact_ = main_stats_contact_cpt_ = args[0];
	break;
      default:
	/* Unknown commands */
	proto_send0 ('?');
	return;
      }
    /* When no error, acknowledge commands */
    proto_send (cmd, size, args);
#undef c
}

int
main (int argc, char **argv)
{
    avr_init (argc, argv);
    main_init ();
    main_loop ();
    return 0;
}
