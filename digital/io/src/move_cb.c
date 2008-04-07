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

/* if the left border is under 500 mm of the actual position do not go there. */
#define MOVE_BORDER_LEVEL 200


/*
 * START =ok=>
 *  => DESIRED_POSITION
 *   Tries to reach a position provided by the user. If the position desired can not be reached, it all try to move on the right or the left.
 */
fsm_branch_t
move__START__ok (void)
{
    asserv_goto (move_data.position_x, move_data.position_y);
    return move_next (START, ok);
}

/*
 * DESIRED_POSITION =failed_or_blocked=>
 * near_left_border => MOVE_ON_RIGHT
 *   Same process as the previous one but on the right.
 * near_right_border => MOVE_ON_LEFT
 *   The robot has failed to reach the position. It shall try another position before trying to reach this one again. It shall go to the on the left only if the left border is the farest one.
 */
fsm_branch_t
move__DESIRED_POSITION__failed_or_blocked (void)
{
    asserv_position_t pos;
    asserv_position_t new_pos;

    asserv_get_position (&pos);
    new_pos = pos;
    new_pos.x += MOVE_BORDER_LEVEL ;

    if (move_can_go_on_left_or_right (pos, new_pos))
      {
	asserv_goto (new_pos.x, new_pos.y);
	return move_next_branch (DESIRED_POSITION, failed_or_blocked, near_left_border);
      }
    else
      {
	new_pos.x = pos.x + MOVE_BORDER_LEVEL;
	// replace this by the correct function.
	asserv_goto (new_pos.x, new_pos.y);
	return move_next_branch (DESIRED_POSITION, failed_or_blocked, near_right_border);
      }
}

/*
 * DESIRED_POSITION =reached=>
 *  => END
 *   The position provided by the user has been reached, the FSM can stop.
 */
fsm_branch_t
move__DESIRED_POSITION__reached (void)
{
    return move_next (DESIRED_POSITION, reached);
}

/*
 * MOVE_ON_RIGHT =failed_or_blocked_or_near_border=>
 *  => MOVE_ON_LEFT
 *   The position is fail again, it will try to reach another one.
 */
fsm_branch_t
move__MOVE_ON_RIGHT__failed_or_blocked_or_near_border (void)
{
    asserv_position_t pos;
    asserv_position_t new_pos;

    asserv_get_position (&pos);
    new_pos.x = pos.x - MOVE_BORDER_LEVEL;
    new_pos.y = pos.y;
    new_pos.a = pos.a;

    if (move_can_go_on_left_or_right (pos, new_pos))
      {
	asserv_goto (new_pos.x, new_pos.y);
      }

    return move_next (MOVE_ON_RIGHT, failed_or_blocked_or_near_border);
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
 * MOVE_ON_LEFT =failed_or_blocked_or_near_border=>
 *  => MOVE_ON_RIGHT
 *   The position is fail again, it will try to reach another one.
 */
fsm_branch_t
move__MOVE_ON_LEFT__failed_or_blocked_or_near_border (void)
{
    asserv_position_t pos;
    asserv_position_t new_pos;

    asserv_get_position (&pos);
    new_pos.x = pos.x + MOVE_BORDER_LEVEL;
    new_pos.y = pos.y;
    new_pos.a = pos.a;
    if (move_can_go_on_left_or_right (pos, new_pos))
      {
	// call the correct function to go to the right.
	asserv_goto (new_pos.x, new_pos.y);
      }

    return move_next (MOVE_ON_LEFT, failed_or_blocked_or_near_border);
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


