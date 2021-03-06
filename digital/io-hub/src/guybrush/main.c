/* main.c */
/* guybrush - Eurobot 2012 AI. {{{
 *
 * Copyright (C) 2012 Nicolas Schodet
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
#include "modules/math/geometry/distance.h"

#include "modules/devices/usdist/usdist.h"

#include "timer.h"
#include "chrono.h"
#include "simu.host.h"

#include "asserv.h"
#include "mimot.h"
#include "beacon.h"
#include "twi_master.h"

#include "contact.h"
#include "output.h"
#include "radar.h"
#include "pressure.h"
#include "logger.h"

#define ANGFSM_NAME AI
#include "angfsm.h"
#ifdef HOST
# include <string.h>
#endif
#include "fsm_queue.h"

#include "path.h"
#include "move.h"
#include "top.h"
#include "bottom_clamp.h"

#include "bot.h"
#include "playground_2012.h"

#include "io.h"

#ifndef HOST
# include <avr/wdt.h>
#endif

/** Our color. */
enum team_color_e team_color;

/** Obstacles positions, updated using radar module. */
vect_t main_obstacles_pos[AC_PATH_OBSTACLES_NB];

/** Number of obstacles in main_obstacles_pos. */
uint8_t main_obstacles_nb;

/** Global demo mode flag. */
uint8_t main_demo;

/** Global US distance sensor activation flag.  Warning: US sensors can not be
 * deactivated once activated or you will get outdated values. */
uint8_t main_usdist;

/** FSM debug mode. */
enum
{
    /** Normal run mode. */
    MAIN_FSM_DEBUG_RUN,
    /** Stop, do not handle any event. */
    MAIN_FSM_DEBUG_STOP,
    /** Step one event then stop. */
    MAIN_FSM_DEBUG_STEP,
};

/** Current FSM debug mode. */
uint8_t main_fsm_debug_mode;

/** Robot position stats. */
static uint8_t main_stats_pos_ = 75, main_stats_pos_cpt_ = 1;

/** Obstacles stats. */
static uint8_t main_stats_obstacles_ = 75, main_stats_obstacles_cpt_ = 1;

/** Asserv stats counters. */
static uint8_t main_stats_asserv_, main_stats_asserv_cpt_;

/** Contact stats counters. */
static uint8_t main_stats_contact_, main_stats_contact_cpt_;

/** US sensors stats counters. */
static uint8_t main_stats_usdist_, main_stats_usdist_cpt_;

/** Pressure stats counters. */
static uint8_t main_stats_pressure_, main_stats_pressure_cpt_;

/** Chrono stats. */
static uint8_t main_stats_chrono_, main_stats_chrono_last_s_;

/** Timer stats. */
static uint8_t main_stats_timer_, main_stats_timer_cpt_;

/** Clamp zero stats. */
static uint8_t main_stats_clamp_zero_;

/** Clamp zero stats: last sensor value. */
static uint8_t main_stats_clamp_zero_last_io_;

/** Clamp zero stats: last position. */
static uint16_t main_stats_clamp_zero_last_position_;

/** Position to drop CD */
static int position_to_drop;

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
    /* IO modules. */
    contact_init ();
    output_init ();
    usdist_init ();
    logger_init ();
    /* Send some debug aid in case of TWI synchronisation failure.
     * If !Z is sent, but not !z, this means that a slave is missing or is not
     * functioning. */
    proto_send0 ('Z');
    /* TWI communications */
    asserv_init ();
    mimot_init ();
    if (AC_AI_TWI_MASTER_BEACON)
	beacon_init ();
    twi_master_init ();
    /* AI modules. */
    path_init ();
    /* Initialization done. */
    proto_send0 ('z');
}

/** Generate demo mode events. */
static uint8_t
main_demo_events (void)
{
    static uint8_t color_switch_last = 0xff;
    static uint8_t nb_robots_switch_last = 0xff;
    static uint8_t tree_step;
    static uint8_t sleep;
    /* Bounce detection. */
    if (sleep)
      {
	sleep--;
	return 0;
      }
    /* Look at color switch to control totem picking code. */
    if (color_switch_last == 0xff)
	color_switch_last = contact_get_color ();
    if (color_switch_last != contact_get_color ())
      {
	color_switch_last = contact_get_color ();
	sleep = 125;
	switch (tree_step)
	  {
	  case 0:
	    clamp_request (FSM_EVENT (tree_detected));
	    break;
	  case 1:
	    clamp_request (FSM_EVENT (empty_tree));
	    break;
	  case 2:
	    clamp_request (FSM_EVENT (robot_is_back));
	    break;
	  }
	tree_step = (tree_step + 1) % 3;
	return 1;
      }
    /* Look at nb robots switch to control unblocking. */
    if (nb_robots_switch_last == 0xff)
	nb_robots_switch_last = IO_GET (CONTACT_NB_ROBOTS);
    if (nb_robots_switch_last != IO_GET (CONTACT_NB_ROBOTS))
      {
	nb_robots_switch_last = IO_GET (CONTACT_NB_ROBOTS);
	sleep = 125;
	return FSM_HANDLE (AI, clamp_unblock);
      }
    return 0;
}

static uint8_t
main_coin_detected_ok (void)
{
    position_t robot_pos;
    asserv_get_position (&robot_pos);
    int16_t limit = robot_pos.v.y < PG_LENGTH - PG_CAPTAIN_ROOM_LENGTH_MM
	? PG_HOLD_NORTH_X + BOT_SIZE_FRONT
	: PG_CAPTAIN_ROOM_LENGTH_MM + BOT_SIZE_FRONT;
    if (main_demo)
	return 1;
    if (robot_pos.v.x < limit || robot_pos.v.x > PG_MIRROR_X (limit))
	return 0;
    if (robot_pos.v.y > PG_COIN_QUARTET_Y - 100
	&& robot_pos.v.y < PG_COIN_QUARTET_Y + 100)
      {
	if (robot_pos.a < POSITION_A_DEG (90)
	    || robot_pos.a > POSITION_A_DEG (270))
	  {
	    if (robot_pos.v.x > PG_COIN_QUARTET_X - BOT_SIZE_FRONT - 170
		&& robot_pos.v.x < PG_COIN_QUARTET_X - BOT_SIZE_FRONT + 170)
		return 0;
	  }
	else
	  {
	    if (robot_pos.v.x > PG_COIN_QUARTET_X + BOT_SIZE_FRONT - 170
		&& robot_pos.v.x < PG_COIN_QUARTET_X + BOT_SIZE_FRONT + 170)
		return 0;
	  }
      }
    return 1;
}

static void
main_obstacles_merge (vect_t *obs_pos, uint8_t obs_nb)
{
    uint8_t i, j;
    for (i = 0; i < obs_nb; i++)
      {
	/* Look at existing obstacles. */
	for (j = 0; j < main_obstacles_nb; j++)
	  {
	    /* If same obstacle, break. */
	    if (distance_point_point (&obs_pos[i],
				      &main_obstacles_pos[j]) < 150)
		break;
	  }
	/* Else, add it. */
	if (j == main_obstacles_nb)
	  {
	    main_obstacles_pos[main_obstacles_nb] = obs_pos[i];
	    main_obstacles_nb++;
	  }
      }
}

/** Main events management. */
uint8_t
main_event_to_fsm (void)
{
    /* If an event is handled, stop generating any other event, because a
     * transition may have invalidated the current robot state. */
#define FSM_HANDLE_E(fsm, event) \
    do { if (FSM_HANDLE (fsm, event)) return 1; } while (0)
#define FSM_HANDLE_VAR_E(fsm, event) \
    do { if (FSM_HANDLE_VAR (fsm, event)) return 1; } while (0)
#define FSM_HANDLE_TIMEOUT_E(fsm) \
    do { if (FSM_HANDLE_TIMEOUT (fsm)) return 1; } while (0)
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
    if (mimot_motor0_status == success)
	FSM_HANDLE_E (AI, lower_clamp_rotation_success);
    else if (mimot_motor0_status == failure)
	FSM_HANDLE_E (AI, lower_clamp_rotation_failure);
    if ((IO_GET (CONTACT_LOWER_CLAMP_SENSOR_1)
	|| IO_GET (CONTACT_LOWER_CLAMP_SENSOR_2))
	&& !clamp_calm_mode_read ()
	&& main_coin_detected_ok ())
	FSM_HANDLE_E (AI, coin_detected);
    if ((int16_t) (mimot_get_motor0_position() - position_to_drop) > 0)
        FSM_HANDLE_E (AI, time_to_drop_coin);
    if (IO_GET(CONTACT_LOWER_CLAMP_ZERO))
        FSM_HANDLE_E (AI, 0_found);
    /* Jack. */
    if (!contact_get_jack ())
	FSM_HANDLE_E (AI, jack_inserted);
    else
	FSM_HANDLE_E (AI, jack_removed);
    if (!IO_GET(CONTACT_UPPER_CLAMP_DOWN))
	FSM_HANDLE_E (AI,upper_set_down);
    if (!IO_GET(CONTACT_UPPER_CLAMP_UP))
	FSM_HANDLE_E (AI,upper_set_up);
    /* Demo mode. */
    if (main_demo && main_demo_events ())
	return 1;
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
    /* Check obstables. */
    if (move_check_obstacles ())
	return 1;
    /* No event handled. */
    return 0;
}

/** Main (and infinite) loop. */
static void
main_loop (void)
{
    uint8_t timer[6];
    while (1)
      {
	/* Wait until next cycle. */
	timer_wait ();
	timer[0] = timer_get ();
	/* Update chrono. */
	chrono_update ();
	/* Is match over? */
	if (chrono_is_match_over ())
	  {

	    /* Open everything. */
	    IO_SET (OUTPUT_UPPER_CLAMP_OPEN);
	    IO_CLR (OUTPUT_LOWER_CLAMP_1_CLOSE);
	    IO_CLR (OUTPUT_LOWER_CLAMP_2_CLOSE);
	    IO_SET (OUTPUT_DOOR_OPEN);
	    IO_CLR (OUTPUT_DOOR_CLOSE);
	    /* Stop motors. */
	    mimot_motor_free (0, 0);
	    mimot_motor_free (1, 0);
	    /* End it and block here indefinitely. */
	    chrono_end_match (42);
	    return;
	  }
	/* Handle commands from UART. */
	while (uart0_poll ())
	    proto_accept (uart0_getc ());
	/* Update IO modules. */
	timer[1] = timer_get ();
	contact_update ();
	if ((main_demo || main_usdist) && usdist_update ())
	  {
	    position_t robot_pos;
	    asserv_get_position (&robot_pos);
	    /* Get obstacles from beacon system. */
	    main_obstacles_nb = 0;
	    if (AC_AI_TWI_MASTER_BEACON)
	      {
		uint8_t i;
		for (i = 0; i < AC_BEACON_POSITION_NB; i++)
		  {
		    if (beacon_get_position
			(i, &main_obstacles_pos[main_obstacles_nb]) > 70)
			main_obstacles_nb++;
		  }
		beacon_send_position (&robot_pos.v);
	      }
	    /* Get obstacles from radar. */
	    vect_t radar_obs_pos[2];
	    uint8_t radar_obs_nb = radar_update (&robot_pos, radar_obs_pos);
	    main_obstacles_merge (radar_obs_pos, radar_obs_nb);
	    /* Set as move obstacles. */
	    move_obstacles_update ();
	    simu_send_pos_report (main_obstacles_pos, main_obstacles_nb, 0);
	    /* Send stats. */
	    if (main_usdist && main_stats_pos_ && !--main_stats_pos_cpt_)
	      {
		proto_send2w ('R', robot_pos.v.x, robot_pos.v.y);
		logger_log (0xef,
			    v16_to_v8 (robot_pos.v.x, 1),
			    v16_to_v8 (robot_pos.v.x, 0),
			    v16_to_v8 (robot_pos.v.y, 1),
			    v16_to_v8 (robot_pos.v.y, 0));
		main_stats_pos_cpt_ = main_stats_pos_;
	      }
	    if (main_obstacles_nb
		&& main_stats_obstacles_ && !--main_stats_obstacles_cpt_)
	      {
		uint8_t i;
		for (i = 0; i < main_obstacles_nb; i++)
		  {
		    proto_send2w ('O', main_obstacles_pos[i].x, main_obstacles_pos[i].y);
		    logger_log (0xe0 | i,
				v16_to_v8 (main_obstacles_pos[i].x, 1),
				v16_to_v8 (main_obstacles_pos[i].x, 0),
				v16_to_v8 (main_obstacles_pos[i].y, 1),
				v16_to_v8 (main_obstacles_pos[i].y, 0));
		  }
		main_stats_obstacles_cpt_ = main_stats_obstacles_;
	      }
	    else if (main_stats_obstacles_cpt_ > 1)
		main_stats_obstacles_cpt_--;
	  }
	pressure_update ();
	logger_update ();
	output_update ();
	/* Update AI modules. */
	timer[2] = timer_get ();
	top_update ();
	path_decay ();
	/* Only manage events if slaves are synchronised. */
	timer[3] = timer_get ();
	uint8_t sync = twi_master_sync ();
	timer[4] = timer_get ();
	if (sync && main_fsm_debug_mode != MAIN_FSM_DEBUG_STOP)
	  {
	    if (main_event_to_fsm ()
		&& main_fsm_debug_mode == MAIN_FSM_DEBUG_STEP)
		main_fsm_debug_mode = MAIN_FSM_DEBUG_STOP;
	  }
	timer[5] = timer_get ();
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
	    proto_send1d ('P', contact_all () | (uint32_t) mimot_get_input () << 24);
	    main_stats_contact_cpt_ = main_stats_contact_;
	  }
	if (main_stats_usdist_ && !--main_stats_usdist_cpt_)
	  {
	    proto_send4w ('U', usdist_mm[0], usdist_mm[1], usdist_mm[2],
			  usdist_mm[3]);
	    main_stats_usdist_cpt_ = main_stats_usdist_;
	  }
	if (main_stats_pressure_ && !--main_stats_pressure_cpt_)
	  {
	    proto_send1w ('F', pressure_get ());
	    main_stats_pressure_cpt_ = main_stats_pressure_;
	  }
	if (main_stats_chrono_
	    && main_stats_chrono_last_s_ != chrono_remaining_time () / 1000)
	  {
	    main_stats_chrono_last_s_ = chrono_remaining_time () / 1000;
	    proto_send1b ('C', main_stats_chrono_last_s_);
	  }
	if (main_stats_timer_ && !--main_stats_timer_cpt_)
	  {
	    proto_send ('T', UTILS_COUNT (timer), timer);
	    main_stats_timer_cpt_ = main_stats_timer_;
	  }
	if (main_stats_clamp_zero_
	    && IO_GET (CONTACT_LOWER_CLAMP_ZERO)
	    != main_stats_clamp_zero_last_io_)
	  {
	    main_stats_clamp_zero_last_io_ = IO_GET (CONTACT_LOWER_CLAMP_ZERO);
	    if (main_stats_clamp_zero_last_io_)
	      {
		uint16_t new_pos = mimot_get_motor0_position ();
		proto_send1w ('Z', new_pos
			      - main_stats_clamp_zero_last_position_);
		main_stats_clamp_zero_last_position_ = new_pos;
	      }
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
      case c ('j', 0):
	/* Simulate jack insertion. */
	fsm_queue_post_event (FSM_EVENT (jack_inserted));
	break;
      case c ('f', 0):
	/* Enter FSM debug mode, then step once. */
	main_fsm_debug_mode = MAIN_FSM_DEBUG_STEP;
	break;
      case c ('t',0):
	/* Simulate tree detection. */
	fsm_queue_post_event (FSM_EVENT (tree_detected));
	break;
      case c ('s',0):
	/* Simulate stop tree approach. */
	fsm_queue_post_event (FSM_EVENT (stop_tree_approach));
	break;
      case c ('e',0):
	/* Simulate the empty tree command. */
	fsm_queue_post_event (FSM_EVENT (empty_tree));
	break;
      case c ('r',0):
	/* Simulate the robot_is_back command. */
	fsm_queue_post_event (FSM_EVENT (robot_is_back));
	break;
      case c ('u',0):
	/* Simulate the unblock command. */
	fsm_queue_post_event (FSM_EVENT (clamp_unblock));
	break;
      case c ('c', 1):
	/* Simulate clean_start (00), clean_catch (01), clean_load (02). */
	if (args[0] == 0)
	    fsm_queue_post_event (FSM_EVENT (clean_start));
	else if (args[0] == 1)
	    fsm_queue_post_event (FSM_EVENT (clean_catch));
	else
	    fsm_queue_post_event (FSM_EVENT (clean_load));
	break;
      case c ('c',0):
	/* Simulate the coin detected command. */
	fsm_queue_post_event (FSM_EVENT (coin_detected));
	break;
      case c ('m', 5):
	/* Go to position.
	 * - 2w: x, y.
	 * - 1b: backward. */
	  {
	    vect_t position = { v8_to_v16 (args[0], args[1]),
		v8_to_v16 (args[2], args[3]) };
	    move_stop ();
	    move_start_noangle (position, args[4], 0);
	  }
	break;
      case c ('w', 0):
	/* Disable all motor control. */
	mimot_reset();
	break;
      case c ('o', 5):
	/* Set/clear/toggle outputs.
	 * - 1d: mask.
	 * - 1b: 01 to set, 00 to clear, 02 to toggle. */
	  {
	    uint32_t mask = v8_to_v32 (args[0], args[1], args[2], args[3]);
	    if (args[4] == 0)
		output_clear (mask);
	    else if (args[4] == 1)
		output_set (mask);
	    else if (args[4] == 2)
		output_toggle (mask);
	    else
	      {
		proto_send0 ('?');
		return;
	      }
	  }
	break;
      case c ('o', 6):
	/* Toggle outputs for a short time.
	 * - 1d: mask.
	 * - 1w: duration. */
	output_toggle_transient (v8_to_v32 (args[0], args[1], args[2],
					    args[3]),
				 v8_to_v16 (args[4], args[5]));
	break;
      case c ('f', 2):
	/* Set low pressure threshold.
	 * 1w: sensor value, 1024 is full scale. */
	pressure_set (v8_to_v16 (args[0], args[1]));
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
      case c ('U', 1):
	/* US sensors stats. */
	main_stats_usdist_ = main_stats_usdist_cpt_ = args[0];
	break;
      case c ('F', 1):
	/* Pressure stats. */
	main_stats_pressure_ = main_stats_pressure_cpt_ = args[0];
	break;
      case c ('C', 1):
	/* Chrono stats.
	 * - b: start chrono. */
	main_stats_chrono_ = 1;
	main_stats_chrono_last_s_ = 0;
	if (args[0])
	    chrono_start ();
	break;
      case c ('T', 1):
	/* Timer stats. */
	main_stats_timer_ = main_stats_timer_cpt_ = args[0];
	break;
      case c ('Z', 1):
	/* Clamp zero stat. */
	main_stats_clamp_zero_ = args[0];
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

void main_set_drop_coin_pos(int pos_to_drop)
{
    position_to_drop = pos_to_drop;
}

int
main (int argc, char **argv)
{
    /* produce AVR's FSM headers. */
    if (ANGFSM_OPTIONS (argc, argv))
        return 0;
    avr_init (argc, argv);
    main_init ();
    main_loop ();
    return 0;
}
