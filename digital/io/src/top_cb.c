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

/*
 * START =ok=>
 *  => GET_SAMPLES
 *   Go to take some samples. The sequence shall be adapt to take the correct number of samples.
 */
fsm_branch_t
top__START__ok (void)
{
    return top_next (START, ok);
}

/*
 * GET_SAMPLES =samples_took_ice_distributor_empty_not_enough_time=>
 *  => GO_TO_GOAL
 *   Some samples had been took and there is not enough time to get some ice.
 */
fsm_branch_t
top__GET_SAMPLES__samples_took_ice_distributor_empty_not_enough_time (void)
{
    return top_next (GET_SAMPLES, samples_took_ice_distributor_empty_not_enough_time);
}

/*
 * GET_SAMPLES =samples_took=>
 *  => GET_ICE
 *   The samples had been took and now the ice is missing.
 */
fsm_branch_t
top__GET_SAMPLES__samples_took (void)
{
    return top_next (GET_SAMPLES, samples_took);
}

/*
 * GET_SAMPLES =collector_full=>
 *  => GO_TO_GOAL
 *   All the room in the robot are full.
 */
fsm_branch_t
top__GET_SAMPLES__collector_full (void)
{
    return top_next (GET_SAMPLES, collector_full);
}

/*
 * GET_SAMPLES =samples_took_ice_distributor_empty_enough_time=>
 *  => GET_ADV_ICE
 *   Go to take ice in the adversary distributor because the one in our part of the table is empty.
 */
fsm_branch_t
top__GET_SAMPLES__samples_took_ice_distributor_empty_enough_time (void)
{
    return top_next (GET_SAMPLES, samples_took_ice_distributor_empty_enough_time);
}

/*
 * GO_TO_GOAL =samples_deposed_ice_distributor_empty=>
 *  => GET_ADV_ICE
 *   Go to get some adversary ice because our distributor is empty.
 */
fsm_branch_t
top__GO_TO_GOAL__samples_deposed_ice_distributor_empty (void)
{
    return top_next (GO_TO_GOAL, samples_deposed_ice_distributor_empty);
}

/*
 * GO_TO_GOAL =samples_deposed=>
 *  => GET_SAMPLES
 *   The samples had been deposed, it shall try to get more samples. This state will call the getsamples FSM and the moveFSM.
 */
fsm_branch_t
top__GO_TO_GOAL__samples_deposed (void)
{
    return top_next (GO_TO_GOAL, samples_deposed);
}

/*
 * GET_ICE =ice_took=>
 *  => GO_TO_GOAL
 *   The ice had been taken and the collector is full. The robot shall go to depose it into the goal.
 */
fsm_branch_t
top__GET_ICE__ice_took (void)
{
    return top_next (GET_ICE, ice_took);
}

/*
 * GET_ADV_ICE =ice_took_collector_not_full=>
 *  => GET_SAMPLES
 *   There is enough time to get some samples and go to the goal.
 */
fsm_branch_t
top__GET_ADV_ICE__ice_took_collector_not_full (void)
{
    return top_next (GET_ADV_ICE, ice_took_collector_not_full);
}

/*
 * GET_ADV_ICE =ice_took=>
 *  => GO_TO_GOAL
 *   The ice has been taken, now the robot shall depose it.
 */
fsm_branch_t
top__GET_ADV_ICE__ice_took (void)
{
    return top_next (GET_ADV_ICE, ice_took);
}


