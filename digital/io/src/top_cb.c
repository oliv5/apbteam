/*
 * THIS IS AN AUTOMATICALLY GENERATED FILE, DO NOT EDIT!
 *
 * Skeleton for top callbacks implementation.
 *
 * 
 */
#include "common.h"
#include "fsm.h"
#include "top_cb.h"
#include "top.h"
#include "getsamples.h"
#include "asserv.h"
#include "gutter.h"

/*
 * BACKWARD =collector_full=>
 *  => GO_TO_GOAL
 *   Go the goal to depose the samples.
 */
fsm_branch_t
top__BACKWARD__collector_full (void)
{
    gutter_start ();
    top_data.sequence_to_do = (~top_data.sequence_to_do) & 0x1f;
    return top_next (BACKWARD, collector_full);
}

/*
 * BACKWARD =ended=>
 *  => GET_ICE
 *   Get ICE to end the sequence.
 */
fsm_branch_t
top__BACKWARD__ended (void)
{
    /* Generic get sample data */
    struct getsamples_data_t get_sample_data;
    get_sample_data.event = TOP_EVENT_ice_took;
    get_sample_data.sample_bitfield = ~top_data.boxes_used; // XXX
    get_sample_data.distributor_y = ICE_DISTRIBUTOR_Y;
    if (top_data.team_color)
      {
	get_sample_data.distributor_x = ICE_DISTRIBUTOR_LEFT;
	getsamples_start (get_sample_data);
      }
    else
      {
	get_sample_data.distributor_x = ICE_DISTRIBUTOR_RIGHT;
	getsamples_start (get_sample_data);
      }
    return top_next (BACKWARD, ended);
}

/*
 * BACKWARD =ice_dist_empty=>
 *  => GET_ADV_ICE
 *   Go to take the ice in the adversary distributor because our is empty.
 */
fsm_branch_t
top__BACKWARD__ice_dist_empty (void)
{
    /* Generic get sample data */
    struct getsamples_data_t get_sample_data;
    get_sample_data.event = TOP_EVENT_ice_took;
    get_sample_data.sample_bitfield = ~top_data.boxes_used; // XXX
    get_sample_data.distributor_y = ICE_DISTRIBUTOR_Y;
    if (top_data.team_color)
      {
	get_sample_data.distributor_x = ICE_DISTRIBUTOR_LEFT;
	getsamples_start (get_sample_data);
      }
    else
      {
	get_sample_data.distributor_x = ICE_DISTRIBUTOR_RIGHT;
	getsamples_start (get_sample_data);
      }
    return top_next (BACKWARD, ice_dist_empty);
}

/*
 * GET_ICE =ice_took=>
 *  => BACKWARD
 *   Go backward to end the take balls.
 */
fsm_branch_t
top__GET_ICE__ice_took (void)
{
    asserv_position_t pos;

    asserv_get_position (&pos);

    asserv_goto (pos.x - 200, pos.y - 200);
    return top_next (GET_ICE, ice_took);
}

/*
 * START =ok=>
 *  => GET_SAMPLES
 *   Go to take some samples. The sequence shall be adapt to take the correct number of samples.
 */
fsm_branch_t
top__START__ok (void)
{
    /* Generic get sample data */
    struct getsamples_data_t get_sample_data;
    get_sample_data.event = TOP_EVENT_samples_took;
    get_sample_data.sample_bitfield = ~top_data.boxes_used; // XXX
    if (top_data.team_color == BLUE_TEAM)
      {
	get_sample_data.distributor_x = BLUE_DISTRIBUTOR_X;
	get_sample_data.distributor_y = BLUE_DISTRIBUTOR_Y;
	getsamples_start (get_sample_data);
      }
    else
      {
	get_sample_data.distributor_x = RED_DISTRIBUTOR_X;
	get_sample_data.distributor_y = RED_DISTRIBUTOR_Y;
	getsamples_start (get_sample_data);
      }
    return top_next (START, ok);
}

/*
 * GO_TO_GOAL =samples_deposed=>
 *  => GET_SAMPLES
 *   The samples had been deposed, it shall try to get more samples. This state will call the getsamples FSM and the moveFSM.
 */
fsm_branch_t
top__GO_TO_GOAL__samples_deposed (void)
{
    /* Generic get sample data */
    struct getsamples_data_t get_sample_data;
    get_sample_data.event = TOP_EVENT_samples_took;
    get_sample_data.sample_bitfield = ~top_data.boxes_used; // XXX
    if (top_data.team_color == BLUE_TEAM)
      {
	get_sample_data.distributor_x = BLUE_DISTRIBUTOR_X;
	get_sample_data.distributor_y = BLUE_DISTRIBUTOR_Y;
	getsamples_start (get_sample_data);
      }
    else
      {
	get_sample_data.distributor_x = RED_DISTRIBUTOR_X;
	get_sample_data.distributor_y = RED_DISTRIBUTOR_Y;
	getsamples_start (get_sample_data);
      }
    return top_next (GO_TO_GOAL, samples_deposed);
}

/*
 * GET_ADV_ICE =ice_took=>
 *  => BACKWARD
 *   The ice has been taken.
 */
fsm_branch_t
top__GET_ADV_ICE__ice_took (void)
{
    asserv_position_t pos;

    asserv_get_position (&pos);

    asserv_goto (pos.x - 200, pos.y - 200);
    return top_next (GET_ADV_ICE, ice_took);
}

/*
 * GET_SAMPLES =samples_took=>
 *  => BACKWARD
 *   The samples had been took and now the ice is missing.
 */
fsm_branch_t
top__GET_SAMPLES__samples_took (void)
{
    // Call the get samples state machine.

    // Blue color.
    /* Generic get sample data */
    struct getsamples_data_t get_sample_data;
    get_sample_data.event = TOP_EVENT_samples_took;
    get_sample_data.sample_bitfield = top_data.sequence; // XXX
    if (top_data.team_color == BLUE_TEAM)
      {
	get_sample_data.distributor_x = BLUE_DISTRIBUTOR_X;
	get_sample_data.distributor_y = BLUE_DISTRIBUTOR_Y;
	getsamples_start (get_sample_data);
      }
    else
      {
	get_sample_data.distributor_x = RED_DISTRIBUTOR_X;
	get_sample_data.distributor_y = RED_DISTRIBUTOR_Y;
	getsamples_start (get_sample_data);
      }
    return top_next (START, ok);
}


