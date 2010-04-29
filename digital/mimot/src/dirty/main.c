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

#include "state.h"

#include "counter.h"
#include "pwm.h"
#include "pos.h"
#include "speed.h"
#include "aux.h"

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
    pwm_init ();
    timer_init ();
    counter_init ();
    uart0_init ();
    twi_proto_init ();
    speed_init ();
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
    main_timer[5] = timer_read ();
    timer_wait ();
    /* Counter update. */
    counter_update ();
    main_timer[0] = timer_read ();
    /* Position control. */
    pos_update ();
    main_timer[1] = timer_read ();
    /* Pwm setup. */
    pwm_update ();
    main_timer[2] = timer_read ();
    /* Compute absolute position. */
    aux_pos_update ();
    /* Compute trajectory. */
    aux_traj_update ();
    /* Speed control. */
    speed_update ();
    main_timer[3] = timer_read ();
    /* Stats. */
    if (main_sequence_ack
	&& (state_aux[0].sequence_ack != state_aux[0].sequence_finish
	    || state_aux[1].sequence_ack != state_aux[1].sequence_finish)
	&& !--main_sequence_ack_cpt)
      {
	proto_send2b ('A', state_aux[0].sequence_finish,
		      state_aux[1].sequence_finish);
	main_sequence_ack_cpt = main_sequence_ack;
      }
    if (main_stat_counter && !--main_stat_counter_cpt)
      {
	proto_send2w ('C', counter_aux[0], counter_aux[1]);
	main_stat_counter_cpt = main_stat_counter;
      }
    if (main_stat_aux_pos && !--main_stat_aux_pos_cpt)
      {
	proto_send2w ('Y', aux[0].pos, aux[1].pos);
	main_stat_aux_pos_cpt = main_stat_aux_pos;
      }
    if (main_stat_speed && !--main_stat_speed_cpt)
      {
	proto_send2b ('S', speed_aux[0].cur >> 8, speed_aux[1].cur >> 8);
	main_stat_speed_cpt = main_stat_speed;
      }
    if (main_stat_pos && !--main_stat_pos_cpt)
      {
	proto_send4w ('P', pos_aux[0].e_old, pos_aux[0].i,
		      pos_aux[1].e_old, pos_aux[1].i);
	main_stat_pos_cpt = main_stat_pos;
      }
    if (main_stat_pwm && !--main_stat_pwm_cpt)
      {
	proto_send2w ('W', pwm_aux[0].cur, pwm_aux[1].cur);
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
    main_timer[4] = timer_read ();
}

/** Handle incoming messages. */
void
proto_callback (uint8_t cmd, uint8_t size, uint8_t *args)
{
    /* Many commands use the first argument as a selector. */
    struct aux_t *auxp = 0;
    struct pwm_t *pwm = 0;
    struct pos_t *pos = 0;
    struct speed_t *speed = 0;
    struct state_t *state = 0;
    if (args[0] < AC_ASSERV_AUX_NB)
      {
	auxp = &aux[args[0]];
	pwm = &pwm_aux[args[0]];
	pos = &pos_aux[args[0]];
	speed = &speed_aux[args[0]];
	state = &state_aux[args[0]];
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
	pos_reset (&pos_aux[0]);
	pos_reset (&pos_aux[1]);
	state_aux[0].mode = MODE_PWM;
	state_aux[1].mode = MODE_PWM;
	pwm_set (&pwm_aux[0], 0);
	pwm_set (&pwm_aux[1], 0);
	break;
      case c ('w', 3):
	/* Set auxiliary pwm.
	 * - b: aux index.
	 * - w: pwm. */
	if (!auxp) { proto_send0 ('?'); return; }
	pos_reset (pos);
	state->mode = MODE_PWM;
	pwm_set (pwm, v8_to_v16 (args[1], args[2]));
	break;
      case c ('c', 3):
	/* Add to auxiliary position consign.
	 * - b: aux index.
	 * - w: consign offset. */
	if (!auxp) { proto_send0 ('?'); return; }
	state->mode = MODE_POS;
	state->variant = 0;
	pos->cons += v8_to_v16 (args[1], args[2]);
	break;
      case c ('s', 1):
	/* Set auxiliary zero speed.
	 * - b: aux index. */
	if (!auxp) { proto_send0 ('?'); return; }
	state->mode = MODE_SPEED;
	state->variant = 0;
	speed->use_pos = 0;
	speed->cons = 0;
	break;
      case c ('s', 2):
	/* Set auxiliary speed.
	 * - b: aux index.
	 * - b: speed. */
	if (!auxp) { proto_send0 ('?'); return; }
	state->mode = MODE_SPEED;
	state->variant = 0;
	speed->use_pos = 0;
	speed->cons = args[1] << 8;
	break;
      case c ('s', 6):
	/* Set auxiliary speed controlled position consign.
	 * - b: aux index.
	 * - d: consign offset.
	 * - b: sequence number. */
	if (!auxp) { proto_send0 ('?'); return; }
	if (args[5] == state->sequence)
	    break;
	speed->use_pos = 1;
	speed->pos_cons = pos->cons;
	speed->pos_cons += v8_to_v32 (args[1], args[2], args[3], args[4]);
	state_start (state, MODE_SPEED, args[5]);
	break;
      case c ('y', 4):
	/* Auxiliary go to position.
	 * - b: aux index.
	 * - w: pos, i16.
	 * - b: sequence number. */
	if (!auxp) { proto_send0 ('?'); return; }
	if (args[3] == state->sequence)
	    break;
	aux_traj_goto_start (auxp, v8_to_v16 (args[1], args[2]), args[3]);
	break;
      case c ('y', 3):
	/* Auxiliary find zero.
	 * - b: aux index.
	 * - b: speed.
	 * - b: sequence number. */
	if (!auxp) { proto_send0 ('?'); return; }
	if (args[2] == state->sequence)
	    break;
	aux_traj_find_zero_start (auxp, args[1], args[2]);
	break;
      case c ('a', 2):
	/* Set all acknoledge.
	 * - b: first auxiliary ack sequence number.
	 * - b: second auxiliary ack sequence number. */
	state_acknowledge (&state_aux[0], args[1]);
	state_acknowledge (&state_aux[1], args[2]);
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
		pos = &pos_aux[args[1]];
		speed = &speed_aux[args[1]];
		break;
	      default:
		pos = 0;
		speed = 0;
		break;
	      }
	    switch (c (args[0], size))
	      {
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
		if (!pos) { proto_send0 ('?'); return; }
		pos->blocked_error_limit = v8_to_v16 (args[2], args[3]);
		pos->blocked_speed_limit = v8_to_v16 (args[4], args[5]);
		pos->blocked_counter_limit = args[6];
		break;
	      case c ('E', 3):
		pos_e_sat = v8_to_v16 (args[1], args[2]);
		break;
	      case c ('I', 3):
		pos_i_sat = v8_to_v16 (args[1], args[2]);
		break;
	      case c ('D', 3):
		pos_d_sat = v8_to_v16 (args[1], args[2]);
		break;
	      case c ('w', 2):
		/* Set PWM direction.
		 * - b: bits: 0000[aux1][aux0][right][left]. */
		pwm_set_reverse (args[1]);
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
		proto_send1b ('E', EEPROM_KEY);
		proto_send2w ('a', speed_aux[0].acc, speed_aux[1].acc);
		proto_send2b ('s', speed_aux[0].max, speed_aux[0].slow);
		proto_send2b ('s', speed_aux[1].max, speed_aux[1].slow);
		proto_send3w ('b', pos_aux[0].blocked_error_limit,
			      pos_aux[0].blocked_speed_limit,
			      pos_aux[0].blocked_counter_limit);
		proto_send3w ('b', pos_aux[1].blocked_error_limit,
			      pos_aux[1].blocked_speed_limit,
			      pos_aux[1].blocked_counter_limit);
		proto_send2w ('p', pos_aux[0].kp, pos_aux[1].kp);
		proto_send2w ('i', pos_aux[0].ki, pos_aux[1].ki);
		proto_send2w ('d', pos_aux[0].kd, pos_aux[1].kd);
		proto_send1w ('E', pos_e_sat);
		proto_send1w ('I', pos_i_sat);
		proto_send1w ('D', pos_d_sat);
		proto_send1b ('w', pwm_reverse);
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

