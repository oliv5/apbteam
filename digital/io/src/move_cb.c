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
#include "move.h"
#include "asserv.h"
#include "main.h"	/* main_post_event_for_top_fsm */

/*
 * IDLE =start=>
 *  => DESIRED_POSITION
 *   Tries to reach a position provided by the user. If the position desired can not be reached, it all try to move on the right or the left.
 */
fsm_branch_t
move__IDLE__start (void)
{
    asserv_goto (move_data.position_x, move_data.position_y);
    return move_next (IDLE, start);
}

/*
 * MOVE_ON_RIGHT =blocked=>
 *  => MOVE_ON_LEFT
 *   The robot will try to go on the left because the previous movement block on the left.
 */
fsm_branch_t
move__MOVE_ON_RIGHT__blocked (void)
{
    move_go_to_left (); 
    return move_next (MOVE_ON_RIGHT, blocked);
}

/*
 * MOVE_ON_RIGHT =reached=>
 *  => DESIRED_POSITION
 *   The position has been reached. It will now try to reach the position provided by the user.
 */
fsm_branch_t
move__MOVE_ON_RIGHT__reached (void)
{
    asserv_goto (move_data.position_x, move_data.position_y);
    return move_next (MOVE_ON_RIGHT, reached);
}

/*
 * DESIRED_POSITION =blocked=>
 * near_right_border => MOVE_ON_LEFT
 *   The robot will compute the position from the border and will take the decision to go on the left because the right border is too near.
 * near_left_border => MOVE_ON_RIGHT
 *   The robot will compute the position from the border and will take the decision to go on the right because the left border is too near.
 */
fsm_branch_t
move__DESIRED_POSITION__blocked (void)
{
    asserv_position_t pos;
    asserv_position_t new_pos;

    asserv_get_position (&pos);
    new_pos = pos;
    new_pos.x += MOVE_BORDER_LEVEL ;

    if (move_can_go_on_left_or_right (pos, new_pos))
      {
	asserv_goto (new_pos.x, new_pos.y);
	return move_next_branch (DESIRED_POSITION, blocked, near_left_border);
      }
    else
      {
	new_pos.x = pos.x + MOVE_BORDER_LEVEL;
	// replace this by the correct function.
	asserv_goto (new_pos.x, new_pos.y);
	return move_next_branch (DESIRED_POSITION, blocked, near_right_border);
      }
}

/*
 * DESIRED_POSITION =reached=>
 *  => IDLE
 *   The position provided by the user has been reached, the FSM can stop.
 */
fsm_branch_t
move__DESIRED_POSITION__reached (void)
{
    main_post_event_for_top_fsm = TOP_EVENT_move_fsm_finished;
    return move_next (DESIRED_POSITION, reached);
}

/*
 * MOVE_ON_LEFT =blocked=>
 *  => MOVE_ON_RIGHT
 *   The robot will try to go on the left because the previous movement block on the right.
 */
fsm_branch_t
move__MOVE_ON_LEFT__blocked (void)
{
    move_go_to_right();
    return move_next (MOVE_ON_LEFT, blocked);
}

/*
 * MOVE_ON_LEFT =reached=>
 *  => DESIRED_POSITION
 *   The position has been reached. It will now try to reach the position provided by the user.
 */
fsm_branch_t
move__MOVE_ON_LEFT__reached (void)
{
    asserv_goto (move_data.position_x, move_data.position_y);
    return move_next (MOVE_ON_LEFT, reached);
}


