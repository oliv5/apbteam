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

#include "cs.h"
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

/** Report of auxiliary position. */
uint8_t main_stat_aux_pos, main_stat_aux_pos_cpt;

/** Statistics about speed control. */
uint8_t main_stat_speed, main_stat_speed_cpt;

/** Statistics about shaft position control. */
uint8_t main_stat_pos, main_stat_pos_cpt;

/** Statistics about pwm values. */
uint8_t main_stat_pwm, main_stat_pwm_cpt;

/** Report of timer. */
uint8_t main_stat_timer, main_stat_timer_cpt;

/** Print input port. */
uint8_t main_print_pin, main_print_pin_cpt;

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
    /* Pull-ups. */
    PORTB = 0xe0;
    PORTC = 0xfc;
    PORTD = 0x80;
    timer_init ();
    uart0_init ();
    twi_proto_init ();
    cs_init ();
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
    aux_pos_update ();
    main_timer[4] = timer_read ();
    /* Compute trajectory. */
    aux_traj_update ();
    /* Prepare control system. */
    cs_update_prepare ();
    main_timer[5] = timer_read ();
    /* Wait for next cycle. */
    timer_wait ();
    /* Encoder update. */
    encoder_update ();
    main_timer[0] = timer_read ();
    /* Control system update. */
    cs_update ();
    main_timer[1] = timer_read ();
    /* Pwm setup. */
    output_update ();
    main_timer[2] = timer_read ();
    /* Sequences. */
    seq_update (&seq_aux[0], &cs_aux[0].state);
    seq_update (&seq_aux[1], &cs_aux[1].state);
    /* Stats. */
    if (main_sequence_ack
	&& (seq_aux[0].ack != seq_aux[0].finish
	    || seq_aux[1].ack != seq_aux[1].finish)
	&& !--main_sequence_ack_cpt)
      {
	//XXX here
	proto_send2b ('A', seq_aux[0].finish, seq_aux[1].finish);
	main_sequence_ack_cpt = main_sequence_ack;
      }
    if (main_stat_counter && !--main_stat_counter_cpt)
      {
	proto_send2w ('C', encoder_aux[0].cur, encoder_aux[1].cur);
	main_stat_counter_cpt = main_stat_counter;
      }
    if (main_stat_aux_pos && !--main_stat_aux_pos_cpt)
      {
	proto_send2w ('Y', aux[0].pos, aux[1].pos);
	main_stat_aux_pos_cpt = main_stat_aux_pos;
      }
    if (main_stat_speed && !--main_stat_speed_cpt)
      {
	proto_send2w ('S', cs_aux[0].speed.cur_f >> 8,
		      cs_aux[1].speed.cur_f >> 8);
	main_stat_speed_cpt = main_stat_speed;
      }
    if (main_stat_pos && !--main_stat_pos_cpt)
      {
	proto_send4w ('P', cs_aux[0].pos.last_error, cs_aux[0].pos.i,
		      cs_aux[1].pos.last_error, cs_aux[1].pos.i);
	main_stat_pos_cpt = main_stat_pos;
      }
    if (main_stat_pwm && !--main_stat_pwm_cpt)
      {
	proto_send2w ('W', output_aux[0].cur, output_aux[1].cur);
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
	proto_send1b ('I', PINC & 0xfc);
	main_print_pin_cpt = main_print_pin;
      }
    /* Misc. */
    while (uart0_poll ())
	proto_accept (uart0_getc ());
    twi_proto_update ();
}

/** Handle incoming messages. */
void
proto_callback (uint8_t cmd, uint8_t size, uint8_t *args)
{
    /* Many commands use the first argument as a selector. */
    struct aux_t *auxp = 0;
    pos_control_t *pos = 0;
    speed_control_t *speed = 0;
    control_state_t *state = 0;
    blocking_detection_t *bd = 0;
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
	/* Set zero auxiliary pwm. */
	output_set (&output_aux[0], 0);
	output_set (&output_aux[1], 0);
	control_state_set_mode (&cs_aux[0].state, CS_MODE_NONE, 0);
	control_state_set_mode (&cs_aux[1].state, CS_MODE_NONE, 0);
	break;
      case c ('w', 3):
	/* Set auxiliary pwm.
	 * - b: aux index.
	 * - w: pwm. */
	if (!auxp) { proto_send0 ('?'); return; }
	output_set (output, v8_to_v16 (args[1], args[2]));
	control_state_set_mode (state, CS_MODE_NONE, 0);
	break;
      case c ('b', 2):
	/* Set brake and set zero auxiliary pwm.
	 * - b: aux index.
	 * - b: brake (1) or not (0). */
	if (!auxp) { proto_send0 ('?'); return; }
	output_brake (output, args[1]);
	output_set (output, 0);
	control_state_set_mode (state, CS_MODE_NONE, 0);
	break;
      case c ('c', 3):
	/* Add to auxiliary position consign.
	 * - b: aux index.
	 * - w: consign offset. */
	if (!auxp) { proto_send0 ('?'); return; }
	pos->cons += v8_to_v16 (args[1], args[2]);
	control_state_set_mode (state, CS_MODE_POS_CONTROL, 0);
	break;
      case c ('s', 1):
	/* Set auxiliary zero speed.
	 * - b: aux index. */
	if (!auxp) { proto_send0 ('?'); return; }
	speed_control_set_speed (speed, 0);
	control_state_set_mode (state, CS_MODE_SPEED_CONTROL, 0);
	break;
      case c ('s', 3):
	/* Set auxiliary speed.
	 * - b: aux index.
	 * - w: speed. */
	if (!auxp) { proto_send0 ('?'); return; }
	speed_control_set_speed (speed, v8_to_v16 (args[1], args[2]));
	control_state_set_mode (state, CS_MODE_SPEED_CONTROL, 0);
	break;
      case c ('s', 6):
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
	break;
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
      case c ('y', 6):
	/* Auxiliary clamp.
	 * - b: aux index.
	 * - w: speed.
	 * - w: clamping PWM.
	 * - b: sequence number. */
	if (!auxp) { proto_send0 ('?'); return; }
	if (!seq_start (seq, args[5]))
	    break;
	aux_traj_clamp_start (auxp, v8_to_v16 (args[1], args[2]),
			      v8_to_v16 (args[3], args[4]));
	break;
      case c ('y', 7):
	/* Auxiliary find zero.
	 * - b: aux index.
	 * - w: speed.
	 * - b: use switch.
	 * - w: reset position.
	 * - b: sequence number. */
	if (!auxp) { proto_send0 ('?'); return; }
	if (!seq_start (seq, args[6]))
	    break;
	if (args[3])
	    aux_traj_find_zero_start (auxp, v8_to_v16 (args[1], args[2]),
				      v8_to_v16 (args[4], args[5]));
	else
	    aux_traj_find_limit_start (auxp, v8_to_v16 (args[1], args[2]),
				       v8_to_v16 (args[4], args[5]));
	break;
      case c ('a', 2):
	/* Set all acknoledge.
	 * - b: first auxiliary ack sequence number.
	 * - b: second auxiliary ack sequence number. */
	seq_acknowledge (&seq_aux[0], args[0]);
	seq_acknowledge (&seq_aux[1], args[1]);
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
      case c ('Y', 1):
	/* Auxiliary position stats. */
	main_stat_aux_pos_cpt = main_stat_aux_pos = args[0];
	break;
      case c ('S', 1):
	/* Motor speed control stats. */
	main_stat_speed_cpt = main_stat_speed = args[0];
	break;
      case c ('P', 1):
	/* Auxiliary motor position control stats. */
	main_stat_pos_cpt = main_stat_pos = args[0];
	break;
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
      default:
	/* Params. */
	if (cmd == 'p')
	  {
	    /* Many commands use the first argument as a selector. */
	    switch (args[1])
	      {
	      case 0:
	      case 1:
		pos = &cs_aux[args[1]].pos;
		speed = &cs_aux[args[1]].speed;
		bd = &cs_aux[args[1]].blocking_detection;
		break;
	      default:
		pos = 0;
		speed = 0;
		bd = 0;
		break;
	      }
	    switch (c (args[0], size))
	      {
	      case c ('a', 4):
		/* Set acceleration.
		 * - b: index.
		 * - w: acceleration. */
		if (!speed) { proto_send0 ('?'); return; }
		speed->acc_f = v8_to_v16 (args[2], args[3]);
		break;
	      case c ('s', 6):
		/* Set maximum and slow speed.
		 * - b: index.
		 * - w: max speed.
		 * - w: slow speed. */
		if (!speed) { proto_send0 ('?'); return; }
		speed->max = v8_to_v16 (args[2], args[3]);
		speed->slow = v8_to_v16 (args[4], args[5]);
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
		cs_aux[0].pos.e_sat =
		    cs_aux[1].pos.e_sat =
		    v8_to_v16 (args[1], args[2]);
		break;
	      case c ('I', 3):
		cs_aux[0].pos.i_sat =
		    cs_aux[1].pos.i_sat =
		    v8_to_v16 (args[1], args[2]);
		break;
	      case c ('D', 3):
		cs_aux[0].pos.d_sat =
		    cs_aux[1].pos.d_sat =
		    v8_to_v16 (args[1], args[2]);
		break;
	      case c ('w', 2):
		/* Set PWM direction.
		 * - b: bits: 000000[aux1][aux0]. */
		output_set_reverse (&output_aux[0], (args[1] & 1) ? 1 : 0);
		output_set_reverse (&output_aux[1], (args[1] & 2) ? 1 : 0);
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
		proto_send1b ('w', (output_aux[0].reverse ? 1 : 0)
			      | (output_aux[1].reverse ? 2 : 0));
		break;
	      case c ('P', 2):
		/* Print current settings for selected control.
		 * - b: index. */
		if (!pos) { proto_send0 ('?'); return; }
		proto_send2b ('E', EEPROM_KEY, eeprom_loaded);
		proto_send1w ('a', speed->acc_f);
		proto_send2w ('s', speed->max, speed->slow);
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

