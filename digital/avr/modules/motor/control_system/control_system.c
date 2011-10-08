/* control_system.c */
/* motor - Motor control module. {{{
 *
 * Copyright (C) 2011 Nicolas Schodet
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
#include "common.h"
#include "control_system.h"

void
control_system_single_init (control_system_single_t *cs)
{
    speed_control_init (&cs->speed, &cs->pos);
    pos_control_init (&cs->pos);
}

void
control_system_polar_init (control_system_polar_t *cs)
{
    speed_control_init (&cs->speed_theta, &cs->pos_theta);
    speed_control_init (&cs->speed_alpha, &cs->pos_alpha);
    pos_control_init (&cs->pos_theta);
    pos_control_init (&cs->pos_alpha);
}

void
control_system_single_update_prepare (control_system_single_t *cs)
{
    uint8_t control_modes = cs->state.modes;
    /* Update speed control. */
    speed_control_update (&cs->speed, control_modes & CS_MODE_SPEED_CONTROL);
}

void
control_system_polar_update_prepare (control_system_polar_t *cs)
{
    uint8_t control_modes = cs->state.modes;
    /* Update speed control. */
    speed_control_update (&cs->speed_theta,
			  control_modes & CS_MODE_SPEED_CONTROL);
    speed_control_update (&cs->speed_alpha,
			  control_modes & CS_MODE_SPEED_CONTROL);
}

void
control_system_single_update (control_system_single_t *cs)
{
    uint8_t control_modes = cs->state.modes;
    int16_t current_speed;
    int16_t out;
    /* Update pos control. */
    current_speed = cs->encoder->diff;
    out = pos_control_update (&cs->pos, current_speed,
			      control_modes & CS_MODE_POS_CONTROL);
    /* Test for blocking condition, detection is done if control is enabled,
     * but action is taken depending on the blocking detection mode. */
    blocking_detection_update (&cs->blocking_detection, current_speed, out,
			       &cs->pos, control_modes & CS_MODE_POS_CONTROL);
    if (control_modes & CS_MODE_BLOCKING_DETECTION
	&& cs->blocking_detection.blocked)
      {
	control_state_blocked (&cs->state);
	/* Reset control so that another action can be done immediately. */
	pos_control_update (&cs->pos, 0, 0);
	blocking_detection_update (&cs->blocking_detection, 0, 0,
				   &cs->pos, 0);
      }
    /* Set output value if under position control, zero on blocking. */
    if (control_modes & CS_MODE_POS_CONTROL)
	output_set (cs->output, out);
    else if (control_modes & CS_MODE_BLOCKED)
	output_set (cs->output, 0);
}

void
control_system_polar_update (control_system_polar_t *cs)
{
    uint8_t control_modes = cs->state.modes;
    int16_t current_speed_theta, current_speed_alpha;
    int16_t out_theta, out_alpha;
    /* Update pos control. */
    current_speed_theta = cs->encoder_right->diff + cs->encoder_left->diff;
    current_speed_alpha = cs->encoder_right->diff - cs->encoder_left->diff;
    out_theta = pos_control_update
	(&cs->pos_theta, current_speed_theta,
	 control_modes & CS_MODE_POS_CONTROL_THETA);
    out_alpha = pos_control_update
	(&cs->pos_alpha, current_speed_alpha,
	 control_modes & CS_MODE_POS_CONTROL_ALPHA);
    /* Test for blocking condition, detection is done if control is enabled,
     * but action is taken depending on the blocking detection mode. */
    blocking_detection_update (&cs->blocking_detection_theta,
			       current_speed_theta, out_theta, &cs->pos_theta,
			       control_modes & CS_MODE_POS_CONTROL_THETA);
    blocking_detection_update (&cs->blocking_detection_alpha,
			       current_speed_alpha, out_alpha, &cs->pos_alpha,
			       control_modes & CS_MODE_POS_CONTROL_ALPHA);
    if (control_modes & CS_MODE_BLOCKING_DETECTION
	&& (cs->blocking_detection_theta.blocked
	    || cs->blocking_detection_alpha.blocked))
      {
	control_state_blocked (&cs->state);
	/* Reset control so that another action can be done immediately. */
	pos_control_update (&cs->pos_theta, 0, 0);
	pos_control_update (&cs->pos_alpha, 0, 0);
	blocking_detection_update (&cs->blocking_detection_theta, 0, 0,
				   &cs->pos_theta, 0);
	blocking_detection_update (&cs->blocking_detection_alpha, 0, 0,
				   &cs->pos_alpha, 0);
      }
    /* Set output value if under position control, zero on blocking. */
    if (control_modes & (CS_MODE_POS_CONTROL_THETA |
			 CS_MODE_POS_CONTROL_ALPHA))
      {
	output_set (cs->output_left, out_theta - out_alpha);
	output_set (cs->output_right, out_theta + out_alpha);
      }
    else if (control_modes & CS_MODE_BLOCKED)
      {
	output_set (cs->output_left, 0);
	output_set (cs->output_right, 0);
      }
}

