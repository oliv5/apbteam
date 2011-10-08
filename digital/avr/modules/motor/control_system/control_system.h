#ifndef control_system_h
#define control_system_h
/* control_system.h */
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

#include "modules/motor/control_state/control_state.h"
#include "modules/motor/encoder/encoder.h"
#include "modules/motor/speed_control/speed_control.h"
#include "modules/motor/pos_control/pos_control.h"
#include "modules/motor/blocking_detection/blocking_detection.h"
#include "modules/motor/output/output.h"

/** Single motor control system. */
struct control_system_single_t
{
    control_state_t state;
    encoder_t *encoder;
    speed_control_t speed;
    pos_control_t pos;
    blocking_detection_t blocking_detection;
    output_t *output;
};
typedef struct control_system_single_t control_system_single_t;

/** Polar control system. */
struct control_system_polar_t
{
    control_state_t state;
    encoder_t *encoder_left;
    encoder_t *encoder_right;
    speed_control_t speed_theta;
    speed_control_t speed_alpha;
    pos_control_t pos_theta;
    pos_control_t pos_alpha;
    blocking_detection_t blocking_detection_theta;
    blocking_detection_t blocking_detection_alpha;
    output_t *output_left;
    output_t *output_right;
};
typedef struct control_system_polar_t control_system_polar_t;

/** Initialise single motor control system components. */
void
control_system_single_init (control_system_single_t *cs);

/** Initialise polar control system components. */
void
control_system_polar_init (control_system_polar_t *cs);

/** Prepare single motor control system update (update all non urgent
 * modules).  To be called before encoder is updated. */
void
control_system_single_update_prepare (control_system_single_t *cs);

/** Prepare polar control system update (update all non urgent modules).  To
 * be called before encoders are updated. */
void
control_system_polar_update_prepare (control_system_polar_t *cs);

/** Update single motor control system components (however do not update
 * encoder, neither output).  To be called after encoder is updated. */
void
control_system_single_update (control_system_single_t *cs);

/** Update polar control system components (however do not update encoder,
 * neither output).  To be called after encoders are updated. */
void
control_system_polar_update (control_system_polar_t *cs);

#endif /* control_system_h */
