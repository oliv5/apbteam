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

#define FSM_NAME AI
#include "fsm.h"
#ifdef HOST
# include <string.h>
#endif
#include "fsm_queue.h"

#include "clamp.h"
#include "logistic.h"

#include "bot.h"

#include "io.h"

#ifndef HOST
# include <avr/wdt.h>
#endif

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
#ifndef HOST
    /* Disable watchdog (enabled in bootloader). */
    MCUSR &= ~(1 << WDRF);
    wdt_disable ();
#endif
    /* Serial port */
    uart0_init ();
    /* Enable interrupts */
    sei ();
    /* Main timer */
    timer_init ();
    timer_wait ();
    /* TWI communications */
    asserv_init ();
    mimot_init ();
    twi_master_init ();
    /* IO modules. */
    pwm_init ();
    contact_init ();
    /* AI modules. */
    logistic_init ();
    /* Initialization done. */
    proto_send0 ('z');
}

/** Main events management. */
void
main_event_to_fsm (void)
{
    /* If an event is handled, stop generating any other event, because a
     * transition may have invalidated the current robot state. */
#define FSM_HANDLE_E(fsm, event) \
    do { if (FSM_HANDLE (fsm, event)) return; } while (0)
#define FSM_HANDLE_VAR_E(fsm, event) \
    do { if (FSM_HANDLE_VAR (fsm, event)) return; } while (0)
#define FSM_HANDLE_TIMEOUT_E(fsm) \
    do { if (FSM_HANDLE_TIMEOUT (fsm)) return; } while (0)
    /* Update FSM timeouts. */
    FSM_HANDLE_TIMEOUT_E (AI);
    /* Motor status. */
    asserv_status_e robot_move_status, mimot_motor0_status,
		    mimot_motor1_status;
    robot_move_status = asserv_move_cmd_status ();
    mimot_motor0_status = mimot_motor0_cmd_status ();
    mimot_motor1_status = mimot_motor1_cmd_status ();
    if (robot_move_status == success)
	FSM_HANDLE_E (AI, robot_move_success);
    else if (robot_move_status == failure)
	FSM_HANDLE_E (AI, robot_move_failure);
    if (mimot_motor0_status == success
	&& mimot_motor1_status == success)
	FSM_HANDLE_E (AI, clamp_elevation_rotation_success);
    else if (mimot_motor0_status == failure)
	FSM_HANDLE_E (AI, clamp_elevation_failure);
    else if (mimot_motor1_status == failure)
	FSM_HANDLE_E (AI, clamp_rotation_failure);
    /* Clamp specific events. */
    if (clamp_handle_event ())
	return;
    /* Jack. */
    if (!contact_get_jack ())
	FSM_HANDLE_E (AI, jack_inserted);
    else
	FSM_HANDLE_E (AI, jack_removed);
    /* Events from the event queue. */
    if (fsm_queue_poll ())
      {
	/* We must post the event at the end of this block because if it is
	 * handled, the function will return and every instruction after will
	 * never be executed. */
	uint8_t save_event = fsm_queue_pop_event ();
	/* Post the event */
	FSM_HANDLE_VAR_E (AI, save_event);
      }

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
	/* Update AI modules. */
	logistic_update ();
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
      case c ('c', 1):
	/* Move clamp.
	 * - 1b: position. */
	clamp_move (args[0]);
	break;
      case c ('c', 2):
	/* Move element using clamp.
	 * - 1b: source.
	 * - 1b: destination. */
	clamp_move_element (args[0], args[1]);
	break;
      case c ('n', 2):
	/* Simulate the presence of a new element.
	 * - 1b: position.
	 * - 1b: type. */
	clamp_new_element (args[0], args[1]);
	break;
      case c ('d', 0):
	/* Open all doors. */
	pwm_set_timed (BOT_PWM_DOOR_FRONT_BOTTOM, BOT_PWM_DOOR_OPEN);
	pwm_set_timed (BOT_PWM_DOOR_FRONT_TOP, BOT_PWM_DOOR_OPEN);
	pwm_set_timed (BOT_PWM_DOOR_BACK_BOTTOM, BOT_PWM_DOOR_OPEN);
	pwm_set_timed (BOT_PWM_DOOR_BACK_TOP, BOT_PWM_DOOR_OPEN);
	break;
      case c ('d', 1):
	/* Drop elements.
	 * - 1b: 00: drop clear, 01: drop forward, 02: drop backward. */
	if (args[0] == 0x00)
	    clamp_drop_clear ();
	else
	    clamp_drop (args[0]);
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
#ifdef HOST
    /* Produce AVR's FSM headers. */
    int i;
    if (argc > 1)
	for (i = 1; i < argc; i++)
	    if (strcmp (argv[i], "--gen") == 0)
	      {
		FSM_GENERATE (AVR, 0);
		return 0;
	      }
#endif
    avr_init (argc, argv);
    main_init ();
    main_loop ();
    return 0;
}
