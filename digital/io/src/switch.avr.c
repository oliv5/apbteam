/* switch.c */
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
#include "common.h"

#include "switch.h"

#include "modules/utils/utils.h"	/* set_bit */
#include "io.h"				/* PORT/PIN, bit_is_set */

/**
 * @defgroup SwitchConfiguration Configuration of the switch module.
 * You can change the value of the defines and variables of this group to
 * configure the module.
 */

/**
 * Color selector switch port.
 */
#define SWITCH_COLOR_PORT PORTD

/**
 * Color selector read register port.
 */
#define SWITCH_COLOR_PIN PIND

/**
 * Color selector switch pin number of the port.
 */
#define SWITCH_COLOR_PIN_NUMBER 6

/**
 * Jack switch port.
 */
#define SWITCH_JACK_PORT PORTD

/**
 * Jack switch read register port.
 */
#define SWITCH_JACK_PIN PIND

/**
 * Jack switch pin number of the port.
 */
#define SWITCH_JACK_PIN_NUMBER 7

/**
 * Number of iteration of 4.4 ms to have for the filter.
 */
#define SWITCH_JACK_FILTER_NUMBER 50

/**
 * Get the current state of the jack switch.
 * @return the state of the jack without any filter.
 */
static inline uint8_t
switch_get_jack_raw (void);

/**
 * The current filtered value of the jack.
 */
uint8_t switch_jack_filtered_value_;

/** @} */

/* Initialize the switch module. */
void
switch_init (void)
{
    /* By default, all pins are in input direction */
    /* Enable the pull-ups */
   set_bit (SWITCH_COLOR_PORT, SWITCH_COLOR_PIN_NUMBER);
   set_bit (SWITCH_JACK_PORT, SWITCH_JACK_PIN_NUMBER);
   /* By default, there is no jack inserted */
   switch_jack_filtered_value_ = 1;
}

/* Update the switch module. */
void
switch_update (void)
{
    /* Filter counter */
    static uint8_t switch_jack_filter = 0;

    /* If the get return the same as the cached one */
    if (switch_get_jack_raw () == switch_jack_filtered_value_)
	/* Ensure filter counter is zero */
	switch_jack_filter = 0;
    else
      {
	/* Increment filter counter and compare it to the max filter */
	if (switch_jack_filter++ == SWITCH_JACK_FILTER_NUMBER)
	  {
	    /* We change the value of the jack */
	    switch_jack_filtered_value_ = !switch_jack_filtered_value_;
	    /* Reset filter counter */
	    switch_jack_filter = 0;
	  }
      }
}

/* Get the current state of the select colors switch. */
enum team_color_e
switch_get_color (void)
{
    if (bit_is_set (SWITCH_COLOR_PIN, SWITCH_COLOR_PIN_NUMBER))
	return 1;
    else
	return 0;
}

/* Get the current state of the jack switch. */
static inline uint8_t
switch_get_jack_raw (void)
{
    if (bit_is_set (SWITCH_JACK_PIN, SWITCH_JACK_PIN_NUMBER))
	return 1;
    else
	return 0;
}

/* Get the value of the jack with filtering. */
uint8_t
switch_get_jack (void)
{
    return switch_jack_filtered_value_;
}

