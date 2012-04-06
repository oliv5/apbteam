/* main.c */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2005 Nicolas Schodet
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
#include "modules/utils/utils.h"
#include "modules/utils/byte.h"
#include "modules/math/fixed/fixed.h"
#include "io.h"

#include "misc.h"

#include "cs.h"
#include "postrack.h"
#include "traj.h"
#include "aux.h"

#include "seq.h"

#include "twi_proto.h"
#include "eeprom.h"

#ifndef HOST
# include "timer.h"
#else
# include "simu.host.h"
#endif

/** Report command completion. */
uint8_t main_sequence_ack = 4, main_sequence_ack_cpt;

/** Report of counters. */
uint8_t main_stat_counter, main_stat_counter_cpt;

/** Report of position. */
uint8_t main_stat_postrack, main_stat_postrack_cpt;

#if AC_ASSERV_AUX_NB
/** Report of auxiliary position. */
uint8_t main_stat_aux_pos, main_stat_aux_pos_cpt;
#endif

/** Statistics about speed control. */
uint8_t main_stat_speed, main_stat_speed_cpt;

/** Statistics about shaft position control. */
uint8_t main_stat_pos, main_stat_pos_cpt;

#if AC_ASSERV_AUX_NB
/** Statistics about auxiliary shaft position control. */
uint8_t main_stat_pos_aux, main_stat_pos_aux_cpt;
#endif

/** Statistics about pwm values. */
uint8_t main_stat_pwm, main_stat_pwm_cpt;

/** Report of timer. */
uint8_t main_stat_timer, main_stat_timer_cpt;

/** Print input port. */
uint8_t main_print_pin, main_print_pin_cpt;

#ifdef HOST
/** Simulation data. */
uint8_t main_simu, main_simu_cpt;
#endif /* HOST */

/** Record timer value at different stage of computing. Used for performance
 * analisys. */
uint8_t main_timer[6];

static void
main_loop (void);

/** Entry point. */
int
main (int argc, char **argv)
{
    avr_init (argc, argv);
    MISC_SETUP;
    LED_SETUP;
    timer_init ();
    uart0_init ();
    twi_proto_init ();
    postrack_init ();
    cs_init ();
    traj_init ();
    aux_init ();
    eeprom_read_params ();
    proto_send0 ('z');
    sei ();
    while (1)
	main_loop ();
    return 0;
}

/** Main loop. */
static void
main_loop (void)
{
    main_timer[3] = timer_read ();
    /* Compute absolute position. */
    postrack_update ();
    aux_pos_update ();
    main_timer[4] = timer_read ();
    /* Compute trajectory. */
    traj_update ();
    aux_traj_update ();
    /* Prepare control system. */
    cs_update_prepare ();
    main_timer[5] = timer_read ();
    /* Wait for next cycle. */
    timer_wait ();
    /* Encoder update. */
    encoder_update ();
    encoder_corrector_update (&encoder_right_corrector, &encoder_right);
    main_timer[0] = timer_read ();
    /* Control system update. */
    cs_update ();
    main_timer[1] = timer_read ();
    /* Pwm setup. */
    output_update ();
    main_timer[2] = timer_read ();
    /* Sequences. */
    seq_update (&seq_main, &cs_main.state);
#if AC_ASSERV_AUX_NB
    seq_update (&seq_aux[0], &cs_aux[0].state);
    seq_update (&seq_aux[1], &cs_aux[1].state);
#endif
    /* Stats. */
    if (main_sequence_ack
	&& (seq_main.ack != seq_main.finish
	    || AUX_OR_0 (seq_aux[0].ack != seq_aux[0].finish
			 || seq_aux[1].ack != seq_aux[1].finish))
	&& !--main_sequence_ack_cpt)
      {
	proto_sendb ('A', seq_main.finish
		     AUX_IF (, seq_aux[0].finish, seq_aux[1].finish));
	main_sequence_ack_cpt = main_sequence_ack;
      }
    if (main_stat_counter && !--main_stat_counter_cpt)
      {
	proto_sendw ('C', encoder_left.cur, encoder_right.cur
		     AUX_IF (, encoder_aux[0].cur, encoder_aux[1].cur));
	main_stat_counter_cpt = main_stat_counter;
      }
    if (main_stat_postrack && !--main_stat_postrack_cpt)
      {
	proto_send3d ('X', postrack_x, postrack_y, postrack_a);
	main_stat_postrack_cpt = main_stat_postrack;
      }
#if AC_ASSERV_AUX_NB
    if (main_stat_aux_pos && !--main_stat_aux_pos_cpt)
      {
	proto_send2w ('Y', aux[0].pos, aux[1].pos);
	main_stat_aux_pos_cpt = main_stat_aux_pos;
      }
#endif
    if (main_stat_speed && !--main_stat_speed_cpt)
      {
	proto_sendb ('S', cs_main.speed_theta.cur >> 8,
		     cs_main.speed_alpha.cur >> 8
		     AUX_IF (, cs_aux[0].speed.cur >> 8,
			     cs_aux[1].speed.cur >> 8));
	main_stat_speed_cpt = main_stat_speed;
      }
    if (main_stat_pos && !--main_stat_pos_cpt)
      {
	proto_send4w ('P', cs_main.pos_theta.last_error,
		      cs_main.pos_theta.i,
		      cs_main.pos_alpha.last_error,
		      cs_main.pos_alpha.i);
	main_stat_pos_cpt = main_stat_pos;
      }
#if AC_ASSERV_AUX_NB
    if (main_stat_pos_aux && !--main_stat_pos_aux_cpt)
      {
	proto_send4w ('Q', cs_aux[0].pos.last_error,
		      cs_aux[0].pos.i,
		      cs_aux[1].pos.last_error,
		      cs_aux[1].pos.i);
	main_stat_pos_aux_cpt = main_stat_pos_aux;
      }
#endif
#ifdef HOST
    if (main_simu && !--main_simu_cpt)
      {
	proto_send3w ('H', (uint16_t) simu_pos_x,
		      (uint16_t) simu_pos_y,
		      (uint16_t) (simu_pos_a * 1024));
	main_simu_cpt = main_simu;
      }
#endif /* HOST */
    if (main_stat_pwm && !--main_stat_pwm_cpt)
      {
	proto_sendw ('W', output_left.cur, output_right.cur
		     AUX_IF (, output_aux[0].cur, output_aux[1].cur));
	main_stat_pwm_cpt = main_stat_pwm;
      }
    if (main_stat_timer && !--main_stat_timer_cpt)
      {
	proto_send6b ('T', main_timer[0], main_timer[2], main_timer[3],
		      main_timer[4], main_timer[4], main_timer[5]);
	main_stat_timer_cpt = main_stat_timer;
      }
    if (main_print_pin && !--main_print_pin_cpt)
      {
	proto_send1b ('I', PINC);
	main_print_pin_cpt = main_print_pin;
      }
    /* Misc. */
    while (uart0_poll ())
	proto_accept (uart0_getc ());
    twi_proto_update ();
    main_timer[4] = timer_read ();
}

/** Handle incoming messages. */
void
proto_callback (uint8_t cmd, uint8_t size, uint8_t *args)
{
    pos_control_t *pos = 0;
    speed_control_t *speed = 0;
    blocking_detection_t *bd = 0;
#if AC_ASSERV_AUX_NB
    /* Many commands use the first argument as a selector. */
    struct aux_t *auxp = 0;
    control_state_t *state = 0;
    output_t *output = 0;
    seq_t *seq = 0;
    if (args[0] < AC_ASSERV_AUX_NB)
      {
	auxp = &aux[args[0]];
	pos = &cs_aux[args[0]].pos;
	speed = &cs_aux[args[0]].speed;
	state = &cs_aux[args[0]].state;
	output = &output_aux[args[0]];
	seq = &seq_aux[args[0]];
      }
#endif
    /* Decode command. */
#define c(cmd, size) (cmd << 8 | size)
    switch (c (cmd, size))
      {
      case c ('z', 0):
	/* Reset. */
	utils_reset ();
	break;
    /* Commands. */
      case c ('w', 0):
	/* Set zero pwm. */
	output_set (&output_left, 0);
	output_set (&output_right, 0);
	control_state_set_mode (&cs_main.state, CS_MODE_NONE, 0);
	break;
#if AC_ASSERV_AUX_NB
      case c ('W', 0):
	/* Set zero auxiliary pwm. */
	output_set (&output_aux[0], 0);
	output_set (&output_aux[1], 0);
	control_state_set_mode (&cs_aux[0].state, CS_MODE_NONE, 0);
	control_state_set_mode (&cs_aux[1].state, CS_MODE_NONE, 0);
	break;
#endif
      case c ('w', 4):
	/* Set pwm.
	 * - w: left pwm.
	 * - w: right pwm. */
	output_set (&output_left, v8_to_v16 (args[0], args[1]));
	output_set (&output_right, v8_to_v16 (args[2], args[3]));
	control_state_set_mode (&cs_main.state, CS_MODE_NONE, 0);
	break;
#if AC_ASSERV_AUX_NB
      case c ('W', 3):
	/* Set auxiliary pwm.
	 * - b: aux index.
	 * - w: pwm. */
	if (!auxp) { proto_send0 ('?'); return; }
	output_set (output, v8_to_v16 (args[1], args[2]));
	control_state_set_mode (state, CS_MODE_NONE, 0);
	break;
#endif
      case c ('c', 4):
	/* Add to position consign.
	 * - w: theta consign offset.
	 * - w: alpha consign offset. */
	cs_main.pos_theta.cons += v8_to_v16 (args[0], args[1]);
	cs_main.pos_alpha.cons += v8_to_v16 (args[2], args[3]);
	control_state_set_mode (&cs_main.state, CS_MODE_POS_CONTROL, 0);
	break;
#if AC_ASSERV_AUX_NB
      case c ('C', 3):
	/* Add to auxiliary position consign.
	 * - b: aux index.
	 * - w: consign offset. */
	if (!auxp) { proto_send0 ('?'); return; }
	pos->cons += v8_to_v16 (args[1], args[2]);
	control_state_set_mode (state, CS_MODE_POS_CONTROL, 0);
	break;
#endif
      case c ('s', 0):
	/* Stop (set zero speed). */
	speed_control_set_speed (&cs_main.speed_theta, 0);
	speed_control_set_speed (&cs_main.speed_alpha, 0);
	control_state_set_mode (&cs_main.state, CS_MODE_SPEED_CONTROL, 0);
	break;
      case c ('s', 2):
	/* Set speed.
	 * - b: theta speed.
	 * - b: alpha speed. */
	speed_control_set_speed (&cs_main.speed_theta, args[0]);
	speed_control_set_speed (&cs_main.speed_alpha, args[1]);
	control_state_set_mode (&cs_main.state, CS_MODE_SPEED_CONTROL, 0);
	break;
#if AC_ASSERV_AUX_NB
      case c ('S', 2):
	/* Set auxiliary speed.
	 * - b: aux index.
	 * - b: speed. */
	if (!auxp) { proto_send0 ('?'); return; }
	speed_control_set_speed (speed, args[1]);
	control_state_set_mode (state, CS_MODE_SPEED_CONTROL, 0);
	break;
#endif
      case c ('s', 9):
	/* Set speed controlled position consign.
	 * - d: theta consign offset.
	 * - d: alpha consign offset.
	 * - b: sequence number. */
	if (!seq_start (&seq_main, args[8]))
	    break;
	speed_control_pos_offset (&cs_main.speed_theta,
				  v8_to_v32 (args[0], args[1], args[2],
					     args[3]));
	speed_control_pos_offset (&cs_main.speed_alpha,
				  v8_to_v32 (args[4], args[5], args[6],
					     args[7]));
	control_state_set_mode (&cs_main.state, CS_MODE_SPEED_CONTROL, 0);
	break;
      case c ('l', 5):
	/* Set linear speed controlled position consign.
	 * - d: consign offset.
	 * - b: sequence number. */
	if (!seq_start (&seq_main, args[4]))
	    break;
	speed_control_pos_offset (&cs_main.speed_theta,
				  v8_to_v32 (args[0], args[1], args[2], args[3]));
	speed_control_pos_offset (&cs_main.speed_alpha, 0);
	traj_speed_start ();
	break;
      case c ('a', 5):
	/* Set angular speed controlled position consign.
	 * - d: angle offset.
	 * - b: sequence number. */
	if (!seq_start (&seq_main, args[4]))
	    break;
	traj_angle_offset_start (v8_to_v32 (args[0], args[1], args[2],
					    args[3]));
	break;
#if AC_ASSERV_AUX_NB
      case c ('S', 6):
	/* Set auxiliary speed controlled position consign.
	 * - b: aux index.
	 * - d: consign offset.
	 * - b: sequence number. */
	if (!auxp) { proto_send0 ('?'); return; }
	if (!seq_start (seq, args[5]))
	    break;
	speed_control_pos_offset (speed, v8_to_v32 (args[1], args[2], args[3],
						    args[4]));
	aux_traj_speed_start (auxp);
	break;
#endif
      case c ('f', 2):
	/* Go to the wall.
	 * - b: 0: forward, 1: backward.
	 * - b: sequence number. */
	if (!seq_start (&seq_main, args[1]))
	    break;
	traj_ftw_start (args[0]);
	break;
      case c ('f', 3):
	/* Go to the wall, using center with a delay.
	 * - b: 0: forward, 1: backward.
	 * - b: delay.
	 * - b: sequence number. */
	if (!seq_start (&seq_main, args[2]))
	    break;
	traj_ftw_start_center (args[0], args[1]);
	break;
      case c ('f', 12):
	/* Push the wall.
	 * - b: 0: forward, 1: backward.
	 * - d: init_x, f24.8.
	 * - d: init_y, f24.8.
	 * - w: init_a, f0.16.
	 * - b: sequence number. */
	  {
	    if (!seq_start (&seq_main, args[11]))
		break;
	    int32_t angle;
	    if (args[9] == 0xff && args[10] == 0xff)
		angle = -1;
	    else
		angle = v8_to_v32 (0, args[9], args[10], 0);
	    traj_ptw_start (args[0],
			    v8_to_v32 (args[1], args[2], args[3], args[4]),
			    v8_to_v32 (args[5], args[6], args[7], args[8]),
			    angle);
	  }
	break;
      case c ('F', 1):
	/* Go to the dispenser.
	 * - b: sequence number. */
	if (!seq_start (&seq_main, args[0]))
	    break;
	traj_gtd_start ();
	break;
      case c ('x', 10):
	/* Go to position.
	 * - d: x, f24.8.
	 * - d: y, f24.8.
	 * - b: backward (see traj.h).
	 * - b: sequence number. */
	if (!seq_start (&seq_main, args[9]))
	    break;
	traj_goto_start (v8_to_v32 (args[0], args[1], args[2], args[3]),
			 v8_to_v32 (args[4], args[5], args[6], args[7]),
			 args[8]);
	break;
      case c ('x', 3):
	/* Go to angle.
	 * - d: a, f0.16.
	 * - b: sequence number. */
	if (!seq_start (&seq_main, args[2]))
	    break;
	traj_goto_angle_start (v8_to_v32 (0, args[0], args[1], 0));
	break;
      case c ('x', 12):
	/* Go to position, then angle.
	 * - d: x, f24.8.
	 * - d: y, f24.8.
	 * - w: a, f0.16.
	 * - b: backward (see traj.h).
	 * - b: sequence number. */
	if (!seq_start (&seq_main, args[11]))
	    break;
	traj_goto_xya_start (v8_to_v32 (args[0], args[1], args[2], args[3]),
			     v8_to_v32 (args[4], args[5], args[6], args[7]),
			     v8_to_v32 (0, args[8], args[9], 0),
			     args[10]);
	break;
#if AC_ASSERV_AUX_NB
      case c ('y', 4):
	/* Auxiliary go to position.
	 * - b: aux index.
	 * - w: pos, i16.
	 * - b: sequence number. */
	if (!auxp) { proto_send0 ('?'); return; }
	if (!seq_start (seq, args[3]))
	    break;
	aux_traj_goto_start (auxp, v8_to_v16 (args[1], args[2]));
	break;
      case c ('y', 3):
	/* Auxiliary find zero.
	 * - b: aux index.
	 * - b: speed.
	 * - b: sequence number. */
	if (!auxp) { proto_send0 ('?'); return; }
	if (!seq_start (seq, args[2]))
	    break;
	if (args[0] == 0)
	    aux_traj_find_limit_start (auxp, args[1]);
	else
	    aux_traj_find_zero_reverse_start (auxp, args[1]);
	break;
      case c ('a', 3):
	/* Set all acknoledge.
	 * - b: main ack sequence number.
	 * - b: first auxiliary ack sequence number.
	 * - b: second auxiliary ack sequence number. */
	seq_acknowledge (&seq_aux[0], args[1]);
	seq_acknowledge (&seq_aux[1], args[2]);
	/* no break; */
#endif
      case c ('a', 1):
	/* Set main acknoledge.
	 * - b: main ack sequence number. */
	seq_acknowledge (&seq_main, args[0]);
	break;
    /* Stats.
     * - b: interval between stats. */
      case c ('A', 1):
	/* Command completion report. */
	main_sequence_ack_cpt = main_sequence_ack = args[0];
	break;
      case c ('C', 1):
	/* Counter stats. */
	main_stat_counter_cpt = main_stat_counter = args[0];
	break;
      case c ('X', 1):
	/* Position stats. */
	main_stat_postrack_cpt = main_stat_postrack = args[0];
	break;
#if AC_ASSERV_AUX_NB
      case c ('Y', 1):
	/* Auxiliary position stats. */
	main_stat_aux_pos_cpt = main_stat_aux_pos = args[0];
	break;
#endif
      case c ('S', 1):
	/* Motor speed control stats. */
	main_stat_speed_cpt = main_stat_speed = args[0];
	break;
      case c ('P', 1):
	/* Motor position control stats. */
	main_stat_pos_cpt = main_stat_pos = args[0];
	break;
#if AC_ASSERV_AUX_NB
      case c ('Q', 1):
	/* Auxiliary motor position control stats. */
	main_stat_pos_aux_cpt = main_stat_pos_aux = args[0];
	break;
#endif
      case c ('W', 1):
	/* Pwm stats. */
	main_stat_pwm_cpt = main_stat_pwm = args[0];
	break;
      case c ('T', 1):
	/* Timing stats. */
	main_stat_timer_cpt = main_stat_timer = args[0];
	break;
      case c ('I', 1):
	/* Input port stats. */
	main_print_pin_cpt = main_print_pin = args[0];
	break;
#ifdef HOST
      case c ('H', 1):
	/* Simulation data. */
	main_simu_cpt = main_simu = args[0];
	break;
#endif /* HOST */
      default:
	/* Params. */
	if (cmd == 'p')
	  {
	    /* Many commands use the first argument as a selector. */
	    switch (args[1])
	      {
	      case 't':
		pos = &cs_main.pos_theta;
		speed = &cs_main.speed_theta;
		bd = &cs_main.blocking_detection_theta;
		break;
	      case 'a':
		pos = &cs_main.pos_alpha;
		speed = &cs_main.speed_alpha;
		bd = &cs_main.blocking_detection_alpha;
		break;
#if AC_ASSERV_AUX_NB
	      case 0:
	      case 1:
		pos = &cs_aux[args[1]].pos;
		speed = &cs_aux[args[1]].speed;
		bd = &cs_aux[args[1]].blocking_detection;
		break;
#endif
	      default:
		pos = 0;
		speed = 0;
		bd = 0;
		break;
	      }
	    switch (c (args[0], size))
	      {
	      case c ('X', 1):
		/* Reset position. */
		postrack_x = 0;
		postrack_y = 0;
		postrack_a = 0;
		break;
	      case c ('X', 5):
		/* Set current x position.
		 * - d: x position. */
		postrack_x = v8_to_v32 (args[1], args[2], args[3], args[4]);
		break;
	      case c ('Y', 5):
		/* Set current y position.
		 * - d: y position. */
		postrack_y = v8_to_v32 (args[1], args[2], args[3], args[4]);
		break;
	      case c ('A', 5):
		/* Set current angle.
		 * - d: angle. */
		postrack_a = v8_to_v32 (args[1], args[2], args[3], args[4]);
		break;
	      case c ('c', 5):
		/* Set right counter correction factor.
		 * - d: factor (f8.24). */
		encoder_corrector_set_correction (&encoder_right_corrector,
						  v8_to_v32 (args[1], args[2],
							     args[3], args[4]));
		break;
	      case c ('f', 3):
		/* Set footing.
		 * - w: footing. */
		postrack_set_footing (v8_to_v16 (args[1], args[2]));
		break;
	      case c ('a', 4):
		/* Set acceleration.
		 * - b: index.
		 * - w: acceleration. */
		if (!speed) { proto_send0 ('?'); return; }
		speed->acc = v8_to_v16 (args[2], args[3]);
		break;
	      case c ('s', 4):
		/* Set maximum and slow speed.
		 * - b: index.
		 * - b: max speed.
		 * - b: slow speed. */
		if (!speed) { proto_send0 ('?'); return; }
		speed->max = args[2];
		speed->slow = args[3];
		break;
	      case c ('p', 4):
		/* Set proportional coefficient.
		 * - b: index.
		 * - w: P coefficient. */
		if (!pos) { proto_send0 ('?'); return; }
		pos->kp = v8_to_v16 (args[2], args[3]);
		break;
	      case c ('i', 4):
		/* Set integral coefficient.
		 * - b: index.
		 * - w: I coefficient. */
		if (!pos) { proto_send0 ('?'); return; }
		pos->ki = v8_to_v16 (args[2], args[3]);
		break;
	      case c ('d', 4):
		/* Set differential coefficient.
		 * - b: index.
		 * - w: D coefficient. */
		if (!pos) { proto_send0 ('?'); return; }
		pos->kd = v8_to_v16 (args[2], args[3]);
		break;
	      case c ('b', 7):
		/* Set blocking detection parameters.
		 * - b: index.
		 * - w: error limit.
		 * - w: speed limit.
		 * - b: counter limit. */
		if (!bd) { proto_send0 ('?'); return; }
		bd->error_limit = v8_to_v16 (args[2], args[3]);
		bd->speed_limit = v8_to_v16 (args[4], args[5]);
		bd->counter_limit = args[6];
		break;
	      case c ('E', 3):
		cs_main.pos_theta.e_sat =
		    cs_main.pos_alpha.e_sat =
#if AC_ASSERV_AUX_NB
		    cs_aux[0].pos.e_sat =
		    cs_aux[1].pos.e_sat =
#endif
		    v8_to_v16 (args[1], args[2]);
		break;
	      case c ('I', 3):
		cs_main.pos_theta.i_sat =
		    cs_main.pos_alpha.i_sat =
#if AC_ASSERV_AUX_NB
		    cs_aux[0].pos.i_sat =
		    cs_aux[1].pos.i_sat =
#endif
		    v8_to_v16 (args[1], args[2]);
		break;
	      case c ('D', 3):
		cs_main.pos_theta.d_sat =
		    cs_main.pos_alpha.d_sat =
#if AC_ASSERV_AUX_NB
		    cs_aux[0].pos.d_sat =
		    cs_aux[1].pos.d_sat =
#endif
		    v8_to_v16 (args[1], args[2]);
		break;
	      case c ('e', 5):
		traj_eps = v8_to_v16 (args[1], args[2]);
		traj_aeps = v8_to_v16 (args[3], args[4]);
		break;
	      case c ('l', 3):
		traj_set_angle_limit (v8_to_v16 (args[1], args[2]));
		break;
	      case c ('w', 2):
		/* Set PWM direction.
		 * - b: bits: 0000[aux1][aux0][right][left]. */
		output_set_reverse (&output_left, (args[1] & 1) ? 1 : 0);
		output_set_reverse (&output_right, (args[1] & 2) ? 1 : 0);
#if AC_ASSERV_AUX_NB
		output_set_reverse (&output_aux[0], (args[1] & 4) ? 1 : 0);
		output_set_reverse (&output_aux[1], (args[1] & 8) ? 1 : 0);
#endif
		break;
	      case c ('E', 2):
		/* Write to eeprom.
		 * - b: 00: clear config, 01: write config. */
		if (args[1])
		    eeprom_write_params ();
		else
		    eeprom_clear_params ();
		break;
	      case c ('P', 1):
		/* Print current settings. */
		proto_send2b ('E', EEPROM_KEY, eeprom_loaded);
		proto_send1d ('c', encoder_right_corrector.correction);
		proto_send1w ('f', postrack_footing);
		proto_send2w ('e', traj_eps, traj_aeps);
		proto_send1w ('l', traj_angle_limit);
		proto_send1b ('w', (output_left.reverse ? 1 : 0)
			      | (output_right.reverse ? 2 : 0)
			      | AUX_OR_0 ((output_aux[0].reverse ? 4 : 0)
					  | (output_aux[1].reverse ? 8 : 0)));
		break;
	      case c ('P', 2):
		/* Print current settings for selected control.
		 * - b: index. */
		if (!pos) { proto_send0 ('?'); return; }
		proto_send2b ('E', EEPROM_KEY, eeprom_loaded);
		proto_send1w ('a', speed->acc);
		proto_send2b ('s', speed->max, speed->slow);
		proto_send3w ('b', bd->error_limit, bd->speed_limit,
			      bd->counter_limit);
		proto_send1w ('p', pos->kp);
		proto_send1w ('i', pos->ki);
		proto_send1w ('d', pos->kd);
		proto_send1w ('E', pos->e_sat);
		proto_send1w ('I', pos->i_sat);
		proto_send1w ('D', pos->d_sat);
		break;
	      default:
		proto_send0 ('?');
		return;
	      }
	  }
#ifdef HOST
	else if (cmd == 'h')
	  {
	    switch (c (args[0], size))
	      {
	      case c ('X', 7):
		/* Set simulated position.
		 * - w: x position.
		 * - w: y position.
		 * - w: angle (rad) * 1024. */
		simu_pos_x = (double) v8_to_v16 (args[1], args[2]);
		simu_pos_y = (double) v8_to_v16 (args[3], args[4]);
		simu_pos_a = (double) (int16_t) v8_to_v16 (args[5], args[6])
		    / 1024;
		break;
	      }
	  }
#endif /* HOST */
	else
	  {
	    proto_send0 ('?');
	    return;
	  }
	break;
      }
    proto_send (cmd, size, args);
#undef c
}

