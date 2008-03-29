/*
 * THIS IS AN AUTOMATICALLY GENERATED FILE, DO NOT EDIT!
 *
 * Skeleton for get_samples callbacks implementation.
 *
 * 
 */
#include "gutter_cb.h"

/*
 * START =ok=>
 *  => GO_TO_GUTTER
 *   Go to the gutter.
 */
gutter_branch_t
gutter__START__ok (gutter_t *user)
{
    asserv_go_to_gutter();
    return gutter_next (START, ok);
}

/*
 * CLOSE_COLLECTOR =collector_closed=>
 *  => END
 *   The samples has been inserted in the gutter.
 */
gutter_branch_t
gutter__CLOSE_COLLECTOR__collector_closed (gutter_t *user)
{
    //Close the collector.
    return gutter_next (CLOSE_COLLECTOR, collector_closed);
}

/*
 * GO_TO_GUTTER =position_failed=>
 *  => GO_TO_GUTTER
 *   The position failed, shall try another path.
 */
gutter_branch_t
gutter__GO_TO_GUTTER__position_failed (gutter_t *user)
{
    return gutter_next (GO_TO_GUTTER, position_failed);
}

/*
 * GO_TO_GUTTER =position_reached=>
 *  => OPEN_COLLECTOR
 *   The robo is near the gutter and the door can be opened.
 */
gutter_branch_t
gutter__GO_TO_GUTTER__position_reached (gutter_t *user)
{
    // Open the collector.
    return gutter_next (GO_TO_GUTTER, position_reached);
}

/*
 * OPEN_COLLECTOR =collector_opened=>
 *  => CLOSE_COLLECTOR
 *   Wait some time and clse the door.
 */
gutter_branch_t
gutter__OPEN_COLLECTOR__collector_opened (gutter_t *user)
{
    utils_delay_ms(4.4);
    return gutter_next (OPEN_COLLECTOR, collector_opened);
}


