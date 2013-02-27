/* hola.c */
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

#define ANGFSM_NAME AI

#include "common.h"
#include "angfsm.h"
#include "asserv.h"
#include "mimot.h"
#include "bot.h"
#include "playground_2010.h"

#include "modules/math/fixed/fixed.h"

uint32_t hola_angle;
int16_t hola_timeout;

FSM_STATES (
	    /* waiting start */
	    HOLA_IDLE,
	    /* wait hola signal */
	    HOLA_WAIT_HOLA,
	    /* wait until the jack is inserted for the first time */
	    HOLA_WAIT_JACK_IN,
	    /* wait until the jack is removed to initialise robot */
	    HOLA_WAIT_JACK_OUT,
	    /* moving clamps in rounds */
	    HOLA_ROUNDS,
	    /* moving clamp up */
	    HOLA_UP,
	    HOLA_CENTER,
	    HOLA_CLAP_CLOSE,
	    HOLA_CLAP_OPEN)

FSM_EVENTS (hola_start)

FSM_START_WITH (HOLA_IDLE)

FSM_TRANS (HOLA_IDLE, start, HOLA_WAIT_HOLA)
{
    return FSM_NEXT (HOLA_IDLE, start);
}

FSM_TRANS (HOLA_WAIT_HOLA, hola_start, HOLA_WAIT_JACK_IN)
{
    return FSM_NEXT (HOLA_WAIT_HOLA, hola_start);
}

FSM_TRANS (HOLA_WAIT_JACK_IN, jack_inserted_into_bot, HOLA_WAIT_JACK_OUT)
{
    return FSM_NEXT (HOLA_WAIT_JACK_IN, jack_inserted_into_bot);
}

FSM_TRANS (HOLA_WAIT_JACK_OUT, jack_removed_from_bot, HOLA_ROUNDS)
{
    return FSM_NEXT (HOLA_WAIT_JACK_OUT, jack_removed_from_bot);
}

FSM_TRANS (HOLA_ROUNDS, asserv_last_cmd_ack,
	   /* move clamp rounds */
	   no_timeout, HOLA_ROUNDS,
	   /* move up, rotate */
	   timeout, HOLA_UP)
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
	return FSM_NEXT (HOLA_ROUNDS, asserv_last_cmd_ack, no_timeout);
      }
    else
      {
	asserv_set_speed (0x10, 0x1c, 0x10, 0x1c);
	asserv_move_motor0_absolute (BOT_ELEVATOR_REST_STEP, BOT_ELEVATOR_SPEED / 3);
	asserv_move_angularly (POSITION_A_DEG (174));
	return FSM_NEXT (HOLA_ROUNDS, asserv_last_cmd_ack, timeout);
      }
}

FSM_TRANS (HOLA_UP, bot_move_succeed, HOLA_CENTER)
{
    asserv_set_speed (0x10, 0x20, 0x10, 0x20);
    asserv_move_angularly (POSITION_A_DEG (-90));
    asserv_move_motor0_absolute (BOT_ELEVATOR_REST_STEP / 2, BOT_ELEVATOR_SPEED);
    return FSM_NEXT (HOLA_UP, bot_move_succeed);
}

FSM_TRANS (HOLA_CENTER, bot_move_succeed, HOLA_CLAP_CLOSE)
{
    mimot_motor0_clamp (BOT_CLAMP_ZERO_SPEED, 0);
    mimot_motor1_clamp (BOT_CLAMP_ZERO_SPEED, 0);
    return FSM_NEXT (HOLA_CENTER, bot_move_succeed);
}

FSM_TRANS (HOLA_CLAP_CLOSE, clamp_succeed, HOLA_CLAP_OPEN)
{
    mimot_move_motor0_absolute (BOT_CLAMP_OPEN_STEP, BOT_CLAMP_SPEED);
    mimot_move_motor1_absolute (BOT_CLAMP_OPEN_STEP, BOT_CLAMP_SPEED);
    return FSM_NEXT (HOLA_CLAP_CLOSE, clamp_succeed);
}

/*
 *   close clamp
 */
FSM_TRANS (HOLA_CLAP_OPEN, clamp_succeed, HOLA_CLAP_CLOSE)
{
    mimot_motor0_clamp (BOT_CLAMP_SPEED, 0);
    mimot_motor1_clamp (BOT_CLAMP_SPEED, 0);
    return FSM_NEXT (HOLA_CLAP_OPEN, clamp_succeed);
}
