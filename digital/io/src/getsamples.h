#ifndef getsamples_h
#define getsamples_h
/* getsamples.h */
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

#include "common.h"

/**
 * Get samples FSM associated data.
 */
struct getsamples_data_t
{
    /**
     * The angle to approach to face the distributor.
     */
    int16_t approach_angle;
    /**
     * Bit-field to indicate where to put the collected samples.
     * For example, if the bit 0 is set to 1, the sample took will be put into
     * the box id 0 (out_right_box). Otherwise, if this bit is set to 0, the
     * out_right box will not be use to store the sample. This parameter is
     * also used to know the number of samples to collect from the
     * distributor.
     */
    uint8_t sample_bitfield;
    /** Position of the arm where we want to be awken. */
    uint16_t arm_noted_position;
    uint8_t direction;
};


/**
 * Start the get samples FSM.
 * @param approach_angle the angle of approach to face the distributor
 * @param sample_bitfield a bit-field to indicate where to put the collected
 * samples. For example, if the bit 0 is set to 1, the sample took will be put
 * into the box id 0 (out_right_box). Otherwise, if this bit is set to 0, the
 * out_right box will not be use to store the sample. This parameter is also
 * used to know the number of samples to collect from the distributor.
 */
void
getsamples_start (int16_t approach_angle, uint8_t sample_bitfield, uint8_t
		  direction);

#endif /* getsamples_h */
