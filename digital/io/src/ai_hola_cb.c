/* ai_hola_cb.c */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2010 Nicolas Schodet
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
#include "fsm.h"
#include "ai_cb.h"
#include "asserv.h"
#include "mimot.h"
#include "bot.h"
#include "playground.h"

#include "modules/math/fixed/fixed.h"

uint32_t hola_angle;
int16_t hola_timeout;

/*
 * HOLA_IDLE =start=>
 *  => HOLA_WAIT_HOLA
 */
fsm_branch_t
ai__HOLA_IDLE__start (void)
{
    return ai_next (HOLA_IDLE, start);
}

/*
 * HOLA_WAIT_HOLA =hola_start=>
 *  => HOLA_WAIT_JACK_IN
 */
fsm_branch_t
ai__HOLA_WAIT_HOLA__hola_start (void)
{
    return ai_next (HOLA_WAIT_HOLA, hola_start);
}

/*
 * HOLA_WAIT_JACK_IN =jack_inserted_into_bot=>
 *  => HOLA_WAIT_JACK_OUT
 */
fsm_branch_t
ai__HOLA_WAIT_JACK_IN__jack_inserted_into_bot (void)
{
    return ai_next (HOLA_WAIT_JACK_IN, jack_inserted_into_bot);
}

/*
 * HOLA_WAIT_JACK_OUT =jack_removed_from_bot=>
 *  => HOLA_ROUNDS
 */
fsm_branch_t
ai__HOLA_WAIT_JACK_OUT__jack_removed_from_bot (void)
{
    return ai_next (HOLA_WAIT_JACK_OUT, jack_removed_from_bot);
}

/*
 * HOLA_ROUNDS =asserv_last_cmd_ack=>
 * no_timeout => HOLA_ROUNDS
 *   move clamp rounds
 * timeout => HOLA_UP
 *   move up
 *   rotate
 */
fsm_branch_t
ai__HOLA_ROUNDS__asserv_last_cmd_ack (void)
{
    if (++hola_timeout < 150)
      {
	int32_t cos = fixed_cos_f824 (hola_angle) + 0x1000000;
	int32_t sin = fixed_sin_f824 (hola_angle) + 0x1000000;
	int32_t dx = BOT_CLAMP_OPEN_STEP
	    + fixed_mul_f824 (BOT_CLAMP_STROKE_STEP / 3 / 2, cos);
	int32_t dy = BOT_ELEVATOR_ZERO_SPEED
	    + fixed_mul_f824 (BOT_ELEVATOR_REST_STEP / 3 / 2, sin);
	mimot_move_motor0_absolute (dx, BOT_CLAMP_SPEED);
	mimot_move_motor1_absolute (dx, BOT_CLAMP_SPEED);
	asserv_move_motor0_absolute (dy, BOT_ELEVATOR_SPEED);
	hola_angle += 0x1000000 / (225 / 12);
	return ai_next_branch (HOLA_ROUNDS, asserv_last_cmd_ack, no_timeout);
      }
    else
      {
	asserv_set_speed (0x10, 0x1c, 0x10, 0x1c);
	asserv_move_motor0_absolute (BOT_ELEVATOR_REST_STEP, BOT_ELEVATOR_SPEED / 3);
	asserv_move_angularly (POSITION_A_DEG (174));
	return ai_next_branch (HOLA_ROUNDS, asserv_last_cmd_ack, timeout);
      }
}

/*
 * HOLA_UP =bot_move_succeed=>
 *  => HOLA_CENTER
 *   rotate to center
 */
fsm_branch_t
ai__HOLA_UP__bot_move_succeed (void)
{
    asserv_set_speed (0x10, 0x20, 0x10, 0x20);
    asserv_move_angularly (POSITION_A_DEG (-90));
    asserv_move_motor0_absolute (BOT_ELEVATOR_REST_STEP / 2, BOT_ELEVATOR_SPEED);
    return ai_next (HOLA_UP, bot_move_succeed);
}

/*
 * HOLA_CENTER =bot_move_succeed=>
 *  => HOLA_CLAP_CLOSE
 *   close clamp
 */
fsm_branch_t
ai__HOLA_CENTER__bot_move_succeed (void)
{
    mimot_motor0_clamp (BOT_CLAMP_ZERO_SPEED, 0);
    mimot_motor1_clamp (BOT_CLAMP_ZERO_SPEED, 0);
    return ai_next (HOLA_CENTER, bot_move_succeed);
}

/*
 * HOLA_CLAP_CLOSE =clamp_succeed=>
 *  => HOLA_CLAP_OPEN
 *   open clamp
 */
fsm_branch_t
ai__HOLA_CLAP_CLOSE__clamp_succeed (void)
{
    mimot_move_motor0_absolute (BOT_CLAMP_OPEN_STEP, BOT_CLAMP_SPEED);
    mimot_move_motor1_absolute (BOT_CLAMP_OPEN_STEP, BOT_CLAMP_SPEED);
    return ai_next (HOLA_CLAP_CLOSE, clamp_succeed);
}

/*
 * HOLA_CLAP_OPEN =clamp_succeed=>
 *  => HOLA_CLAP_CLOSE
 *   close clamp
 */
fsm_branch_t
ai__HOLA_CLAP_OPEN__clamp_succeed (void)
{
    mimot_motor0_clamp (BOT_CLAMP_SPEED, 0);
    mimot_motor1_clamp (BOT_CLAMP_SPEED, 0);
    return ai_next (HOLA_CLAP_OPEN, clamp_succeed);
}


