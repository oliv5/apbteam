/* loader.c */
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
#define FSM_NAME AI

#include "common.h"
#include "loader.h"
#include "fsm.h"
#include "asserv.h"
#include "mimot.h"
#include "bot.h"
#include "fsm_queue.h"
#include "food.h"

uint8_t loader_elements;
uint8_t loader_want_up;

void
loader_up (void)
{
    loader_want_up = 1;
    FSM_HANDLE (AI, loader_up);
}

void
loader_down (void)
{
    loader_want_up = 0;
    FSM_HANDLE (AI, loader_down);
}

FSM_STATES (
	    /* wait start */
	    LOADER_IDLE,
	    /* wait until the jack is inserted for the first time */
	    LOADER_WAIT_JACK_IN,
	    /* wait until the jack is removed to initialise robot */
	    LOADER_WAIT_JACK_OUT,
	    /* move the elevator down to find zero position */
	    LOADER_INIT_ELEVATOR_ZERO,
	    /* test clamp closing */
	    LOADER_INIT_CLAMP_CLOSE,
	    /* find clamp zero position */
	    LOADER_INIT_CLAMP_ZERO,
	    /* find gate zero */
	    LOADER_INIT_GATE_ZERO,
	    /* move the elevator up so that the robot can go to the wall */
	    LOADER_INIT_ELEVATOR_UP,
	    /* wait with gate open */
	    LOADER_INIT_GATE_WAIT,
	    /* close gate before start */
	    LOADER_INIT_GATE_CLOSE,
	    /* up position, clamp is protected */
	    LOADER_UP,
	    /* down position, ready to take an element */
	    LOADER_DOWN,
	    /* moving to the up position */
	    LOADER_UPING,
	    /* moving to the down position */
	    LOADER_DOWNING,
	    /* error while moving up, stop in an unknown state */
	    LOADER_ERROR,
	    /* error recovery moving down */
	    LOADER_ERROR_DOWNING,
	    /* error recovery moving down, open clamp */
	    LOADER_ERROR_DOWNING_OPEN,
	    /* error recovery moving up */
	    LOADER_ERROR_UPING,
	    /* error recovery moving up, open clamp */
	    LOADER_ERROR_UPING_OPEN,
	    /* close clamp */
	    LOADER_LOAD_CLOSING,
	    /* move load up to the unload position */
	    LOADER_LOAD_UPING,
	    /* open clamp to unload, wait until unload position */
	    LOADER_LOAD_UNLOADING,
	    /* check clamp is open */
	    LOADER_LOAD_UNLOADING_OPEN,
	    /* open clamp due to no elements to pick */
	    LOADER_LOAD_EMPTY_OPEN)

FSM_EVENTS (
	    /* asserv success result */
	    elevator_succeed,
	    /* asserv failure result */
	    elevator_failed,
	    /* elevator above unloading position */
	    elevator_unload_position,
	    /* asserv failure result */
	    clamp_failed,
	    /* asserv success result */
	    gate_succeed,
	    /* asserv failure result */
	    gate_failed,
	    /* element sensed between clamp to be loaded */
	    loader_element,
	    /* make the loader ready to load an element */
	    loader_down,
	    /* protect the clamp by moving it up */
	    loader_up,
	    /* posted when loader successfully moved down */
	    loader_downed,
	    /* posted when loader successfully moved up */
	    loader_uped,
	    /* posted when loader in an unknown state due to error while moving up */
	    loader_errored,
	    /* loader blocked, due to a black corn */
	    loader_black)

FSM_START_WITH (LOADER_IDLE)

FSM_TRANS (LOADER_IDLE, start, LOADER_WAIT_JACK_IN)
{
    return FSM_NEXT (LOADER_IDLE, start);
}

FSM_TRANS (LOADER_WAIT_JACK_IN,
	   jack_inserted_into_bot,
	   LOADER_WAIT_JACK_OUT)
{
    return FSM_NEXT (LOADER_WAIT_JACK_IN, jack_inserted_into_bot);
}

FSM_TRANS (LOADER_WAIT_JACK_IN,
	   hola_start,
	   LOADER_IDLE)
{
    return FSM_NEXT (LOADER_WAIT_JACK_IN, hola_start);
}

/*
 * find elevator zero
 * close clamp
 * find gate zero
 */
FSM_TRANS (LOADER_WAIT_JACK_OUT,
	   jack_removed_from_bot,
	   LOADER_INIT_ELEVATOR_ZERO)
{
    asserv_motor0_zero_position (-BOT_ELEVATOR_ZERO_SPEED);
    mimot_motor0_clamp (BOT_CLAMP_ZERO_SPEED, 0);
    mimot_motor1_clamp (BOT_CLAMP_ZERO_SPEED, 0);
    asserv_motor1_zero_position (-BOT_GATE_SPEED);
    return FSM_NEXT (LOADER_WAIT_JACK_OUT, jack_removed_from_bot);
}

FSM_TRANS (LOADER_INIT_ELEVATOR_ZERO,
	   elevator_succeed,
	   LOADER_INIT_CLAMP_CLOSE)
{
    return FSM_NEXT (LOADER_INIT_ELEVATOR_ZERO, elevator_succeed);
}

/*
 * move elevator up
 * find clamp zero
 */
FSM_TRANS (LOADER_INIT_CLAMP_CLOSE,
	   clamp_succeed,
	   LOADER_INIT_CLAMP_ZERO)
{
    asserv_move_motor0_absolute (BOT_ELEVATOR_REST_STEP,
				 BOT_ELEVATOR_ZERO_SPEED);
    mimot_motor0_zero_position (-BOT_CLAMP_ZERO_SPEED);
    mimot_motor1_zero_position (-BOT_CLAMP_ZERO_SPEED);
    return FSM_NEXT (LOADER_INIT_CLAMP_CLOSE, clamp_succeed);
}

FSM_TRANS (LOADER_INIT_CLAMP_ZERO,
	   clamp_succeed,
	   LOADER_INIT_GATE_ZERO)
{
    return FSM_NEXT (LOADER_INIT_CLAMP_ZERO, clamp_succeed);
}

FSM_TRANS (LOADER_INIT_GATE_ZERO,
	   gate_succeed,
	   LOADER_INIT_ELEVATOR_UP)
{
    return FSM_NEXT (LOADER_INIT_GATE_ZERO, gate_succeed);
}

FSM_TRANS (LOADER_INIT_ELEVATOR_UP,
	   elevator_succeed,
	   LOADER_INIT_GATE_WAIT)
{
    return FSM_NEXT (LOADER_INIT_ELEVATOR_UP, elevator_succeed);
}

/*
 * initialisation failure
 */
FSM_TRANS (LOADER_INIT_ELEVATOR_UP,
	   elevator_failed,
	   LOADER_IDLE)
{
    /* Move so that the operator notice it. */
    asserv_move_motor0_absolute (BOT_ELEVATOR_STROKE_STEP / 3,
				 BOT_ELEVATOR_ZERO_SPEED);
    return FSM_NEXT (LOADER_INIT_ELEVATOR_UP, elevator_failed);
}

/*
 * close gate
 */
FSM_TRANS_TIMEOUT (LOADER_INIT_GATE_WAIT, 225,
		   LOADER_INIT_GATE_CLOSE)
{
    asserv_move_motor1_absolute (BOT_GATE_STROKE_STEP, BOT_GATE_SPEED);
    return FSM_NEXT_TIMEOUT (LOADER_INIT_GATE_WAIT);
}

FSM_TRANS (LOADER_INIT_GATE_CLOSE,
	   gate_succeed,
	   LOADER_UP)
{
    return FSM_NEXT (LOADER_INIT_GATE_CLOSE, gate_succeed);
}

/*
 * move down
 */
FSM_TRANS (LOADER_UP,
	   loader_down,
	   LOADER_DOWNING)
{
    asserv_move_motor0_absolute (BOT_ELEVATOR_DOWN_STEP, BOT_ELEVATOR_SPEED);
    return FSM_NEXT (LOADER_UP, loader_down);
}

/*
 * move up
 */
FSM_TRANS (LOADER_DOWN,
	   loader_up,
	   LOADER_UPING)
{
    asserv_move_motor0_absolute (BOT_ELEVATOR_REST_STEP, BOT_ELEVATOR_SPEED);
    return FSM_NEXT (LOADER_DOWN, loader_up);
}

/*
 * clamp
 */
FSM_TRANS (LOADER_DOWN,
	   loader_element,
	   LOADER_LOAD_CLOSING)
{
    mimot_motor0_clamp (BOT_CLAMP_SPEED, BOT_CLAMP_PWM);
    mimot_motor1_clamp (BOT_CLAMP_SPEED, BOT_CLAMP_PWM);
    return FSM_NEXT (LOADER_DOWN, loader_element);
}

/*
 * post loader_uped event
 */
FSM_TRANS (LOADER_UPING,
	   elevator_succeed,
	   LOADER_UP)
{
    fsm_queue_post_event (FSM_EVENT (AI,loader_uped));
    return FSM_NEXT (LOADER_UPING, elevator_succeed);
}

/*
 * post loader_errored event
 */
FSM_TRANS (LOADER_UPING,
	   elevator_failed,
	   LOADER_ERROR)
{
    fsm_queue_post_event (FSM_EVENT (AI, loader_errored));
    return FSM_NEXT (LOADER_UPING, elevator_failed);
}

/*
 * move down
 */
FSM_TRANS (LOADER_UPING,
	   loader_down,
	   LOADER_DOWNING)
{
    asserv_move_motor0_absolute (BOT_ELEVATOR_DOWN_STEP, BOT_ELEVATOR_SPEED);
    return FSM_NEXT (LOADER_UPING, loader_down);
}

/*
 * release elevator motor
 * post loader_downed event
 */
FSM_TRANS (LOADER_DOWNING,
	   elevator_succeed,
	   LOADER_DOWN)
{
    asserv_motor0_free ();
    fsm_queue_post_event (FSM_EVENT (AI, loader_downed));
    return FSM_NEXT (LOADER_DOWNING, elevator_succeed);
}

/*
 * post loader_errored event
 */
FSM_TRANS (LOADER_DOWNING,
	   elevator_failed,
	   LOADER_ERROR)
{
    fsm_queue_post_event (FSM_EVENT (AI, loader_errored));
    return FSM_NEXT (LOADER_DOWNING, elevator_failed);
}

/*
 * emergency stop
 * post loader_errored event
 */
FSM_TRANS (LOADER_DOWNING,
	   loader_element,
	   LOADER_ERROR)
{
    asserv_motor0_free ();
    fsm_queue_post_event (FSM_EVENT (AI, loader_errored));
    return FSM_NEXT (LOADER_DOWNING, loader_element);
}

/*
 * move up
 */
FSM_TRANS (LOADER_DOWNING,
	   loader_up,
	   LOADER_UPING)
{
    asserv_move_motor0_absolute (BOT_ELEVATOR_REST_STEP, BOT_ELEVATOR_SPEED);
    return FSM_NEXT (LOADER_DOWNING, loader_up);
}

/*
 * move down
 */
FSM_TRANS (LOADER_ERROR,
	   loader_down,
	   LOADER_ERROR_DOWNING)
{
    asserv_move_motor0_absolute (BOT_ELEVATOR_DOWN_STEP, BOT_ELEVATOR_SPEED);
    return FSM_NEXT (LOADER_ERROR, loader_down);
}

/*
 * move up
 */
FSM_TRANS (LOADER_ERROR,
	   loader_up,
	   LOADER_ERROR_UPING)
{
    asserv_move_motor0_absolute (BOT_ELEVATOR_STROKE_STEP, BOT_ELEVATOR_SPEED);
    return FSM_NEXT (LOADER_ERROR, loader_up);
}

/*
 * release elevator motor
 * open clamp
 */
FSM_TRANS (LOADER_ERROR_DOWNING,
	   elevator_succeed,
	   LOADER_ERROR_DOWNING_OPEN)
{
    asserv_motor0_free ();
    mimot_move_motor0_absolute (BOT_CLAMP_OPEN_STEP, BOT_CLAMP_SPEED);
    mimot_move_motor1_absolute (BOT_CLAMP_OPEN_STEP, BOT_CLAMP_SPEED);
    return FSM_NEXT (LOADER_ERROR_DOWNING, elevator_succeed);
}

/*
 * post loader_errored event
 */
FSM_TRANS (LOADER_ERROR_DOWNING,
	   elevator_failed,
	   LOADER_ERROR)
{
    fsm_queue_post_event (FSM_EVENT (AI, loader_errored));
    return FSM_NEXT (LOADER_ERROR_DOWNING, elevator_failed);
}

/*
 * post loader_errored event
 */
FSM_TRANS_TIMEOUT (LOADER_ERROR_DOWNING, 225,
		   LOADER_ERROR)
{
    fsm_queue_post_event (FSM_EVENT (AI, loader_errored));
    return FSM_NEXT_TIMEOUT (LOADER_ERROR_DOWNING);
}

/*
 * post loader_downed event
 */
FSM_TRANS (LOADER_ERROR_DOWNING_OPEN,
	   clamp_succeed,
	   LOADER_DOWN)
{
    fsm_queue_post_event (FSM_EVENT (AI, loader_downed));
    return FSM_NEXT (LOADER_ERROR_DOWNING_OPEN, clamp_succeed);
}

/*
 * post loader_errored event
 */
FSM_TRANS (LOADER_ERROR_DOWNING_OPEN,
	   clamp_failed,
	   LOADER_ERROR)
{
    fsm_queue_post_event (FSM_EVENT (AI, loader_errored));
    return FSM_NEXT (LOADER_ERROR_DOWNING_OPEN, clamp_failed);
}

/*
 * post loader_errored event
 */
FSM_TRANS_TIMEOUT (LOADER_ERROR_DOWNING_OPEN, 225,
		   LOADER_ERROR)
{
    fsm_queue_post_event (FSM_EVENT (AI, loader_errored));
    return FSM_NEXT_TIMEOUT (LOADER_ERROR_DOWNING_OPEN);
}

/*
 * open clamp
 */
FSM_TRANS (LOADER_ERROR_UPING,
	   elevator_succeed,
	   LOADER_ERROR_UPING_OPEN)
{
    mimot_move_motor0_absolute (BOT_CLAMP_OPEN_STEP, BOT_CLAMP_SPEED);
    mimot_move_motor1_absolute (BOT_CLAMP_OPEN_STEP, BOT_CLAMP_SPEED);
    return FSM_NEXT (LOADER_ERROR_UPING, elevator_succeed);
}

/*
 * post loader_errored event
 */
FSM_TRANS (LOADER_ERROR_UPING,
	   elevator_failed,
	   LOADER_ERROR)
{
    fsm_queue_post_event (FSM_EVENT (AI, loader_errored));
    return FSM_NEXT (LOADER_ERROR_UPING, elevator_failed);
}

/*
 * post loader_errored event
 */
FSM_TRANS_TIMEOUT (LOADER_ERROR_UPING, 225,
		   LOADER_ERROR)
{
    fsm_queue_post_event (FSM_EVENT (AI, loader_errored));
    return FSM_NEXT_TIMEOUT (LOADER_ERROR_UPING);
}

/*
 * post loader_uped event
 */
FSM_TRANS (LOADER_ERROR_UPING_OPEN,
	   clamp_succeed,
	   LOADER_UP)
{
    fsm_queue_post_event (FSM_EVENT (AI, loader_uped));
    return FSM_NEXT (LOADER_ERROR_UPING_OPEN, clamp_succeed);
}

/*
 * post loader_errored event
 */
FSM_TRANS (LOADER_ERROR_UPING_OPEN,
	   clamp_failed,
	   LOADER_ERROR)
{
    fsm_queue_post_event (FSM_EVENT (AI, loader_errored));
    return FSM_NEXT (LOADER_ERROR_UPING_OPEN, clamp_failed);
}

/*
 * post loader_errored event
 */
FSM_TRANS_TIMEOUT (LOADER_ERROR_UPING_OPEN, 225,
		   LOADER_ERROR)
{
    fsm_queue_post_event (FSM_EVENT (AI, loader_errored));
    return FSM_NEXT_TIMEOUT (LOADER_ERROR_UPING_OPEN);
}

FSM_TRANS (LOADER_LOAD_CLOSING, clamp_succeed,
	   /* move up
	    * count one element */
	   full, LOADER_LOAD_UPING,
	   /* open clamp */
	   empty, LOADER_LOAD_EMPTY_OPEN)
{
    /* Measure load using clamp position. */
    uint16_t tickness = BOT_CLAMP_WIDTH_STEP
	- mimot_get_motor0_position ()
	- mimot_get_motor1_position ();
    if (tickness > BOT_CLAMP_EMPTY_STEP)
      {
	asserv_move_motor0_absolute (BOT_ELEVATOR_STROKE_STEP,
				     BOT_ELEVATOR_SPEED);
	position_t robot_position;
	asserv_get_position (&robot_position);
	food_taken (robot_position);
	loader_elements++;
	return FSM_NEXT (LOADER_LOAD_CLOSING, clamp_succeed, full);
      }
    else
      {
	mimot_move_motor0_absolute (BOT_CLAMP_OPEN_STEP, BOT_CLAMP_SPEED);
	mimot_move_motor1_absolute (BOT_CLAMP_OPEN_STEP, BOT_CLAMP_SPEED);
	return FSM_NEXT (LOADER_LOAD_CLOSING, clamp_succeed, empty);
      }
}

/*
 * open clamp
 */
FSM_TRANS (LOADER_LOAD_UPING,
	   elevator_unload_position,
	   LOADER_LOAD_UNLOADING)
{
    mimot_move_motor0_absolute (BOT_CLAMP_OPEN_STEP, BOT_CLAMP_SPEED);
    mimot_move_motor1_absolute (BOT_CLAMP_OPEN_STEP, BOT_CLAMP_SPEED);
    return FSM_NEXT (LOADER_LOAD_UPING, elevator_unload_position);
}

/*
 * post loader_errored or loader_black event
 * remove one element
 * open clamp
 */
FSM_TRANS (LOADER_LOAD_UPING,
	   elevator_failed,
	   LOADER_ERROR)
{
    if (asserv_get_motor0_position () < BOT_ELEVATOR_BLACK_THRESHOLD_STEP)
      {
	if (loader_elements)
	    loader_elements--;
	fsm_queue_post_event (FSM_EVENT (AI, loader_black));
      }
    else
	fsm_queue_post_event (FSM_EVENT (AI, loader_errored));
    mimot_move_motor0_absolute (BOT_CLAMP_OPEN_STEP, BOT_CLAMP_SPEED);
    mimot_move_motor1_absolute (BOT_CLAMP_OPEN_STEP, BOT_CLAMP_SPEED);
    return FSM_NEXT (LOADER_LOAD_UPING, elevator_failed);
}

FSM_TRANS (LOADER_LOAD_UNLOADING,
	   elevator_succeed,
	   LOADER_LOAD_UNLOADING_OPEN)
{
    return FSM_NEXT (LOADER_LOAD_UNLOADING, elevator_succeed);
}

/*
 * post loader_errored event
 * open clamp
 */
FSM_TRANS (LOADER_LOAD_UNLOADING,
	   elevator_failed,
	   LOADER_ERROR)
{
    fsm_queue_post_event (FSM_EVENT (AI, loader_errored));
    mimot_move_motor0_absolute (BOT_CLAMP_OPEN_STEP, BOT_CLAMP_SPEED);
    mimot_move_motor1_absolute (BOT_CLAMP_OPEN_STEP, BOT_CLAMP_SPEED);
    return FSM_NEXT (LOADER_LOAD_UNLOADING, elevator_failed);
}

FSM_TRANS (LOADER_LOAD_UNLOADING_OPEN, clamp_succeed,
	   /* move down */
	   down, LOADER_DOWNING,
	   /* move up */
	   up, LOADER_UPING)
{
    if (loader_want_up)
      {
	asserv_move_motor0_absolute (BOT_ELEVATOR_REST_STEP,
				     BOT_ELEVATOR_SPEED);
	return FSM_NEXT (LOADER_LOAD_UNLOADING_OPEN, clamp_succeed, up);
      }
    else
      {
	asserv_move_motor0_absolute (BOT_ELEVATOR_DOWN_STEP,
				     BOT_ELEVATOR_SPEED);
	return FSM_NEXT (LOADER_LOAD_UNLOADING_OPEN, clamp_succeed, down);
      }
}

/*
 * post loader_errored event
 */
FSM_TRANS (LOADER_LOAD_UNLOADING_OPEN,
	   clamp_failed,
	   LOADER_ERROR)
{
    fsm_queue_post_event (FSM_EVENT (AI, loader_errored));
    return FSM_NEXT (LOADER_LOAD_UNLOADING_OPEN, clamp_failed);
}

FSM_TRANS (LOADER_LOAD_EMPTY_OPEN, clamp_succeed,
	   /* post loader_downed event */
	   down, LOADER_DOWN,
	   /* move up */
	   up, LOADER_UPING)
{
    if (loader_want_up)
      {
	asserv_move_motor0_absolute (BOT_ELEVATOR_REST_STEP,
				     BOT_ELEVATOR_SPEED);
	return FSM_NEXT (LOADER_LOAD_EMPTY_OPEN, clamp_succeed, up);
      }
    else
      {
	fsm_queue_post_event (FSM_EVENT (AI, loader_downed));
	return FSM_NEXT (LOADER_LOAD_EMPTY_OPEN, clamp_succeed, down);
      }
}

/*
 * post loader_errored event
 */
FSM_TRANS (LOADER_LOAD_EMPTY_OPEN,
	   clamp_failed,
	   LOADER_ERROR)
{
    fsm_queue_post_event (FSM_EVENT (AI, loader_errored));
    return FSM_NEXT (LOADER_LOAD_EMPTY_OPEN, clamp_failed);
}
