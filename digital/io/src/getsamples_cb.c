/*
 * THIS IS AN AUTOMATICALLY GENERATED FILE, DO NOT EDIT!
 *
 * Skeleton for get_samples callbacks implementation.
 *
 * 
 */
#include "getsamples_cb.h"

/*
 * PREPARE_ARM =arm_moved=>
 *  => FORWARD_CONTROL
 *   Prepare the arm to the correct position.
 */
getsamples_branch_t
getsamples__PREPARE_ARM__arm_moved (getsamples_t *user)
{
    // Try to end the position to the distributor.
    asserv_set_x_position (user->distributor_x);
    // Go to the color distributor.
    asserv_set_y_position (user->distributor_y);
    return getsamples_next (PREPARE_ARM, arm_moved);
}

/*
 * FORWARD_CONTROL =position_reached=>
 *  => TAKE_SAMPLES
 *   End the position to the distributor.
 */
getsamples_branch_t
getsamples__FORWARD_CONTROL__position_reached (getsamples_t *user)
{
    // Take as many samples as necessary. This shall be updated to verify the
    asserv_move_arm (1666, 100);
    return getsamples_next (FORWARD_CONTROL, position_reached);
}

/*
 * START =ok=>
 *  => GO_TO_POSITION
 *   Go to the distributor. The distributor to reach shall be setted in the fsm structure.
 */
getsamples_branch_t
getsamples__START__ok (getsamples_t *user)
{
    asserv_set_x_position (user->distributor_x - 20);
    asserv_set_y_position (user->distributor_y - 20);
    return getsamples_next (START, ok);
}

/**
 * TAKE_SAMPLES =sample_took=>
 * no_more => BACKWARD
 *   If the quantity of samples are tooked, then go backeward and conitnue classifying the samples.
 * more => TAKE_SAMPLES
 *   Continue to take samples and classify the next sample.
 */
getsamples_branch_t
getsamples__TAKE_SAMPLES__sample_took (getsamples_t *user)
{
    // Decrement the samples counter.
    if (user->samples)
      {
	asserv_move_arm (1666, 100);
	return getsamples_next_branch (TAKE_SAMPLES, sample_took, more);
      }
    else
      {
	asserv_move_arm (5000, 100);
	// Try to end the position to the distributor.
	asserv_set_x_position (user->distributor_x - 20);
	// Go to the color distributor.
	asserv_set_y_position (user->distributor_y - 20);

	return getsamples_next_branch (TAKE_SAMPLES, sample_took, no_more);
      }
}

/*
 * BACKWARD =position_reached=>
 *  => END
 *   Ending this state machine.
 */
getsamples_branch_t
getsamples__BACKWARD__position_reached (getsamples_t *user)
{
    return getsamples_next (BACKWARD, position_reached);
}

/*
 * GO_TO_POSITION =position_failed=>
 *  => GO_TO_POSITION
 *   Go to another point before trying to go to this one again.
 */
getsamples_branch_t
getsamples__GO_TO_POSITION__position_failed (getsamples_t *user)
{
    // TODO In this case i don't know what to do.
    return getsamples_next (GO_TO_POSITION, position_failed);
}

/*
 * GO_TO_POSITION =position_reached=>
 *  => PREPARE_ARM
 *   Go to the position desired, it is very near the position of the distributor in case it is a ice distributor or sample distributor.
 */
getsamples_branch_t
getsamples__GO_TO_POSITION__position_reached (getsamples_t *user)
{
    // Put the ARM in the position to allow the robot to take samples from the
    // distributor.
    asserv_move_arm (625, 100);
    return getsamples_next (GO_TO_POSITION, position_reached);
}


