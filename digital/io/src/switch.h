#ifndef switch_h
#define switch_h
/* switch.h */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2008 Dufour Jérémy
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
 * @file Module to manage 'switchs'. For example, colors selector and jack.
 */

#include "giboulee.h"			/* team_color_e */
#include "common.h"

/**
 * Initialize the switch module.
 * This functions just put the pins in input direction and enable pull-ups.
 */
void
switch_init (void);

/**
 * Update the switch module.
 * This function must be called at each "main cycle" to ensure that the filter
 * of the jack is updated.
 */
void
switch_update (void);

/**
 * Get the current state of the select colors switch.
 * Be careful, the result of this function is not filtered.
 * @return the color of our team.
 */
enum team_color_e
switch_get_color (void);

/**
 * Get the filtered value of the jack.
 * In comparison with the \a switch_get_jack_raw, this function ensure you the
 * state of the jack is the same during a defined period of time.
 * @return the filtered state of the jack.
 */
uint8_t
switch_get_jack (void);

#endif /* switch_h */
