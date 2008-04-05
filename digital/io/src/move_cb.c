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

/*
 * START =ok=>
 *  => DESIRED_POSITION
 *   Tries to reach a position provided by the user. If the position desired can not be reached, it all try to move on the right or the left.
 */
fsm_branch_t
move__START__ok (void)
{
    return move_next (START, ok);
}

/*
 * DESIRED_POSITION =failed_or_blocked=>
 *  => MOVE_ON_LEFT
 *   The robot has failed to reach the position. It shall try another position before trying to reach this one again. It shall go to the on the left only if the left border is the farest one.
 */
fsm_branch_t
move__DESIRED_POSITION__failed_or_blocked (void)
{
    return move_next (DESIRED_POSITION, failed_or_blocked);
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
 * MOVE_ON_RIGHT =failed_or_blocked=>
 *  => MOVE_ON_RIGHT
 *   The position is fail again, it will try to reach another one.
 */
fsm_branch_t
move__MOVE_ON_RIGHT__failed_or_blocked (void)
{
    return move_next (MOVE_ON_RIGHT, failed_or_blocked);
}

/*
 * MOVE_ON_RIGHT =reached=>
 *  => DESIRED_POSITION
 *   The position has been reached. It will now try to reach the position provided by the user.
 */
fsm_branch_t
move__MOVE_ON_RIGHT__reached (void)
{
    return move_next (MOVE_ON_RIGHT, reached);
}

/*
 * MOVE_ON_RIGHT =near_border=>
 *  => MOVE_ON_LEFT
 *   The robot is now too near of the border.
 */
fsm_branch_t
move__MOVE_ON_RIGHT__near_border (void)
{
    return move_next (MOVE_ON_RIGHT, near_border);
}

/*
 * MOVE_ON_LEFT =failed_or_blocked=>
 *  => MOVE_ON_LEFT
 *   The position is fail again, it will try to reach another one.
 */
fsm_branch_t
move__MOVE_ON_LEFT__failed_or_blocked (void)
{
    return move_next (MOVE_ON_LEFT, failed_or_blocked);
}

/*
 * MOVE_ON_LEFT =reached=>
 *  => DESIRED_POSITION
 *   The position has been reached. It will now try to reach the position provided by the user.
 */
fsm_branch_t
move__MOVE_ON_LEFT__reached (void)
{
    return move_next (MOVE_ON_LEFT, reached);
}

/*
 * MOVE_ON_LEFT =near_border=>
 *  => MOVE_ON_RIGHT
 *   The robot is now too near of the border.
 */
fsm_branch_t
move__MOVE_ON_LEFT__near_border (void)
{
    return move_next (MOVE_ON_LEFT, near_border);
}


