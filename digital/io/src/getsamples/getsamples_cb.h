#ifndef getsamples_cb_h
#define getsamples_cb_h
/*
 * THIS IS AN AUTOMATICALLY GENERATED FILE, DO NOT EDIT!
 *
 * get_samples callbacks declaration.
 *
 * 
 */

#include "getsamples.h"

/*
 * PREPARE_ARM =arm_prepared=>
 *  => PREPARE_CLASSIFIER
 *   Prepare the arm to load the samples.
 */
getsamples_branch_t
getsamples__PREPARE_ARM__arm_prepared (getsamples_t *user);

/*
 * FORWARD_CONTROL =move_finished=>
 *  => PREPARE_ARM
 *   The position is reached the arm shall be seted to the position disired.
 */
getsamples_branch_t
getsamples__FORWARD_CONTROL__move_finished (getsamples_t *user);

/*
 * GO_TO_POSITION =move_blocked=>
 *  => GO_TO_POSITION
 */
getsamples_branch_t
getsamples__GO_TO_POSITION__move_blocked (getsamples_t *user);

/*
 * GO_TO_POSITION =move_finished=>
 *  => FORWARD_CONTROL
 *   Go to the position desired, it is very near the position of the distributor in case it is a ice distributor or sample distributor.
 */
getsamples_branch_t
getsamples__GO_TO_POSITION__move_finished (getsamples_t *user);

/*
 * PREPARE_CLASSIFIER =classifer_prepared=>
 *  => LOAD_SAMPLES
 *   Load some samples.
 */
getsamples_branch_t
getsamples__PREPARE_CLASSIFIER__classifer_prepared (getsamples_t *user);

#endif /* getsamples_cb_h */
