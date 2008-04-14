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

/**
 * Get samples FSM associated data.
 */
struct getsamples_data_t
{
    /**
     * X position of the distributor where to get samples from.
     */
    uint32_t distributor_x;
    /**
     * Y position of the distributor where to get samples from.
     */
    uint32_t distributor_y;
    /**
     * Bit field to indicate where to put the sample.
     * If bit 0 is set to 1, a sample will be put into the out_right_box. If
     * set to 0, the out_right_box will not be used to store a sample.
     */
    uint8_t sample_bitfield;
    /**
     * Event to post to the top FSM when this one is finished.
     */
    uint8_t event;
};

/**
 * Get samples shared data.
 */
extern struct getsamples_data_t getsamples_data;

/**
 * Start the get samples FSM.
 * @param data get sample data initial configuration.
 */
void
getsamples_start (struct getsamples_data_t data);

/**
 * Configure the classifier (using the trap and the internal bit field) for
 * the first bit set to 1.
 * After the configuring the classifier, the bit will be reset to 0 to use the
 * next one when calling this function again.
 */
void
getsamples_configure_classifier (void);

#endif /* getsamples_h */
