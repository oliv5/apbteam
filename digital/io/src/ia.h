#ifndef ia_h
#define ia_h
/* ia.h */
/*  {{{
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
ia_load_balls (uint8_t balls_nb);

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
 */
void
ia_get_samples (void);

#endif /* ia_h */
