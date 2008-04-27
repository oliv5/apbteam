/*
 * THIS IS AN AUTOMATICALLY GENERATED FILE, DO NOT EDIT!
 *
 * Skeleton for move callbacks implementation.
 *
 * 
 */
#include "common.h"
#include "fsm.h"
#include "move_cb.h"
#include "path.h"
#include "asserv.h"
#include "playground.h"
#include "move.h"

/*
 * GO_AWAY =bot_move_succeed=>
 * position_intermediary => GO_AWAY
 *   Request the next step to reached to the path finder.
 * position_desired => IDLE
 *   The position desired provided on the FSM call had been reached.
 */
fsm_branch_t
move__GO_AWAY__bot_move_succeed (void)
{
    if (move_data.position_x == move_data.path_pos_x 
	&& move_data.position_y == move_data.path_pos_y)
	return move_next_branch (GO_AWAY, bot_move_succeed, position_desired);
    else
      {
	path_endpoints (move_data.path_pos_x, move_data.path_pos_y,
			move_data.position_x, move_data.position_y);
	path_update ();

	path_get_next (&move_data.path_pos_x, &move_data.path_pos_y);
	asserv_goto (move_data.path_pos_x, move_data.path_pos_y);
	return move_next_branch (GO_AWAY, bot_move_succeed, position_intermediary);
      }
}

/*
 * GO_AWAY =bot_move_failed=>
 *  => BACKWARD
 *   Store the current position, and go backward.
 */
fsm_branch_t
move__GO_AWAY__bot_move_failed (void)
{
    asserv_move_linearly (-PG_MOVE_DISTANCE); 
    return move_next (GO_AWAY, bot_move_failed);
}

/*
 * GO_AWAY =bot_move_obstacle=>
 *  => STOP_WAIT
 *   Go to the stop wait event.
 */
fsm_branch_t
move__GO_AWAY__bot_move_obstacle (void)
{
    asserv_position_t obstacle_pos;

    asserv_stop_motor ();

    // Store the obstacle in the path finder.
    asserv_get_position (&obstacle_pos);
    path_obstacle (move_data.nb_obstacle, obstacle_pos.x,
		   obstacle_pos.y, MOVE_OBSTACLE_RADIUS, 
		   MOVE_OBSTACLE_VALIDITY);
    move_data.nb_obstacle ++;

    // TODO: Set flag on main to wait.
    return move_next (GO_AWAY, bot_move_obstacle);
}

/*
 * STOP_WAIT =wait_finished=>
 *  => GO_AWAY
 *   Launch the pathfinder and go to the next position.
 */
fsm_branch_t
move__STOP_WAIT__wait_finished (void)
{
    asserv_position_t current_pos;

    // get the current position.
    asserv_get_position (&current_pos);

    // configure the path finder.
    path_endpoints (current_pos.x, current_pos.y,
		    move_data.position_x, move_data.position_y);
    path_update();

    path_get_next (&move_data.path_pos_x, &move_data.path_pos_y);
    asserv_goto (move_data.path_pos_x, move_data.path_pos_y);
    return move_next (STOP_WAIT, wait_finished);
}

/*
 * IDLE =start=>
 *  => DESIRED_POSITION
 *   Save the destination position and put obstacles number to 0.
 */
fsm_branch_t
move__IDLE__start (void)
{
    // Try to go the desired position.
    asserv_goto (move_data.position_x, move_data.position_y);
    return move_next (IDLE, start);
}

/*
 * BACKWARD =bot_move_succeed=>
 *  => GO_AWAY
 *   Launch the pathfinder and go to the next position.
 */
fsm_branch_t
move__BACKWARD__bot_move_succeed (void)
{
    asserv_position_t current_pos;

    // Configure the path finder.
    asserv_get_position (&current_pos);
    path_endpoints (current_pos.x, current_pos.y,
		    move_data.position_x, move_data.position_y);
    path_update ();

    // Get the next point to reach.
    path_get_next (&move_data.path_pos_x, &move_data.path_pos_y);
    asserv_goto (move_data.path_pos_x, move_data.path_pos_y);
    return move_next (BACKWARD, bot_move_succeed);
}

/*
 * DESIRED_POSITION =bot_move_failed=>
 *  => BACKWARD
 *   Store the current position, initialise the path system i.e. provide the position of the obstacle and request the path system the next position.
 */
fsm_branch_t
move__DESIRED_POSITION__bot_move_failed (void)
{
    // Go backward linearly.
    asserv_move_linearly (-PG_MOVE_DISTANCE); 

    return move_next (DESIRED_POSITION, bot_move_failed);
}

/*
 * DESIRED_POSITION =bot_move_succeed=>
 *  => IDLE
 *   Destination position reached by the bot. Set the flag of the top event to inform it.
 */
fsm_branch_t
move__DESIRED_POSITION__bot_move_succeed (void)
{
    return move_next (DESIRED_POSITION, bot_move_succeed);
}

/*
 * DESIRED_POSITION =bot_move_obstacle=>
 *  => STOP_WAIT
 *   Stop the bot.
 */
fsm_branch_t
move__DESIRED_POSITION__bot_move_obstacle (void)
{
    asserv_position_t obstacle_pos;
    
    asserv_stop_motor ();

    // Store the obstacle in the path finder.
    asserv_get_position (&obstacle_pos);
    path_obstacle (move_data.nb_obstacle, obstacle_pos.x,
		   obstacle_pos.y, MOVE_OBSTACLE_RADIUS, 
		   MOVE_OBSTACLE_VALIDITY);
    move_data.nb_obstacle ++;

    return move_next (DESIRED_POSITION, bot_move_obstacle);
}

