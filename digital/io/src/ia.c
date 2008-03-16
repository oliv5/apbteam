/* ia.c */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2008 Nélio Laranjeiro
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
#include "ia.h"
#include "asserv.h"
#include "modules/proto/proto.h"

static ia_t ia_global;

/** Initialize the IA.
  */
void
ia_init (void)
{
    ia_global.sequence = false;
    ia_global.ice_status_our = false;
}

/** Load balls procedure from a distributor.
  * 
  * 1. Rotate the arm to the desired position to allow the robot to load x
  * balls.
  * 2. Go backward, this will allow the robot to continue rotating the arm and
  * load the balls.
  * 3. Stop the arm and put it to its initial position to disallow the robot
  * to take undesired balls.
  *
  * See trunk/digital/io/doc/loadballs.png (use the make command before)
  * 
  * \param  balls_nb  the quantity of ball to load.
  */
void
ia_load_samples (uint8_t balls_nb)
{
    /* Start the rotation of the arm */
    asserv_move_arm (ASSERV_ARM_ROTATION_FULL , ASSERV_ARM_SPEED_HALF);

    /* Activate the classifier. */
    // TODO write the code to use the classifier. How to know when the
    // quantity of desired samples are loaded ?

    /* At this moment the samples shall be loaded. */
    /* Go backaward */
    asserv_move_linearly (-10);

    /* Set the arm to the initial position. */
    asserv_move_arm (ASSERV_ARM_ROTATION_INIT, ASSERV_ARM_SPEED_FULL);
}

/** Get samples procedure. Request the robot to go and get some sample of the
 * team color or ice samples.
 *
 * 1. Go to the position in front of the distributor.
 * 2. Prepare the arm to get samples.
 * 3. Go forward with a controled speed to get the samples.
 * 4. Prepare the classifier to classify the samples.
 * 5. loadballs with the quantity of samples desired.
 * 6. Continue classifier
 *
 * See trunk/digital/io/doc/getSamples.png and
 * trunk/digital/io/doc/loadballs.png (use make to get the png files).
 *
 * \param  blue  the team color true if the robot is in the blue team.
 */
void
ia_get_samples (bool blue, bool ice)
{
    /* Set the ARM to the init position to avoid the robot to take samples on
     * the distributor path. */
    asserv_move_arm (ASSERV_ARM_ROTATION_INIT, ASSERV_ARM_ROTATION_FULL);

    /* Go to the distributor. */

    if (ice)
      {
	//TODO this will only work when the robot is in the red team, the need to
	//know at which position the robot starts is necessary.
	if (blue)
	    asserv_set_x_position (DISTRIBUTOR_SAMPLES_BLUE_X);
	else
	    asserv_set_x_position (DISTRIBUTOR_SAMPLES_RED_X);

	asserv_set_y_position (DISTRIBUTRO_SAMPLES_Y - 100);
      }
    else
      {
	/* Go to the ice distributor. */
	if (ia_global.ice_status_our)
	    asserv_set_x_position (0);
	else
	    asserv_set_x_position (TABLE_MAX_X);
	
	asser_set_y_position (DISTRIBUTOR_ICE_Y);
      }


    /* Set the classifier to the correct position. */
    /* TODO fill this part */

    /* Poll for the position. */
    /* TODO A function for that ?? */

    ia_load_samples (3 /* TODO change this value to the correct one.*/);
}

/** Get ice.
  */
void
ia_get_ice (void)
{
    load_samples (true, true);
}


/** Depose the samples in the gutter.
  */
void
ia_depose_samples (void)
{
    asserv_go_to_gutter();

    /*TODO open the collector. */

    utils_delay_ms (4);

    /* TODO close the collector. */
}
