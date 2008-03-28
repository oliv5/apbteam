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

#include "io.h"				/* PORT/PIN, bit_is_set */
#include "modules/utils/utils.h"	/* set_bit */

/**
 * @defgroup SwitchConfiguration Configuration of the switch module.
 * You can change the value of the defines and variables of this group to
 * configure the module.
 */

/**
 * Color selector switch port.
 */
#define SWITCH_COLOR_PORT PORTC

/**
 * Color selector read register port.
 */
#define SWITCH_COLOR_PIN PINC

/**
 * Color selector switch pin number of the port.
 */
#define SWITCH_COLOR_PIN_NUMBER 0

/**
 * Jack switch port.
 */
#define SWITCH_JACK_PORT PORTC

/**
 * Jack switch read register port.
 */
#define SWITCH_JACK_PIN PINC

/**
 * Jack switch pin number of the port.
 */
#define SWITCH_JACK_PIN_NUMBER 1

/** @} */

/**
 * Initialize the switch module.
 * This functions just put the pins in input direction and enable pull-ups.
 */
inline void
switch_init (void)
{
    /* By default, all pins are in input direction */
    /* Enable the pull-ups */
   set_bit (SWITCH_COLOR_PORT, SWITCH_COLOR_PIN_NUMBER);
   set_bit (SWITCH_JACK_PORT, SWITCH_JACK_PIN_NUMBER);
}

/**
 * Get the current state of the select colors switch.
 */
inline uint8_t
switch_get_color (void)
{
    return bit_is_set (SWITCH_COLOR_PIN, SWITCH_COLOR_PIN_NUMBER);
}

/**
 * Get the current state of the jack switch.
 */
inline uint8_t
switch_get_jack (void)
{
    return bit_is_set (SWITCH_JACK_PIN, SWITCH_JACK_PIN_NUMBER);
}

#endif /* switch_h */
