/* sharp.c */
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
#include "sharp.h"

#include "modules/adc/adc.h"	/* ADC functions */
#include "io.h"

/**
 * Cached array of raw sharp values.
 */
uint16_t sharp_values_[SHARP_NUMBER];

/**
 * Low (0 index) and high (1 index) thresholds for interpreted sharp values.
 */
uint16_t sharp_threshold[SHARP_NUMBER][2];

/**
 * Filter counter for each sharp.
 */
uint8_t sharp_filter_[SHARP_NUMBER];

/**
 * Filter number value before changing state.
 */
#define SHARP_FILTER_NUMBER 3

/**
 * Previous sharp interpreted values.
 */
uint8_t sharp_previous_values_[SHARP_NUMBER];

/* Initialize sharp module. */
void
sharp_init (void)
{
    /* Pins are in input mode by default */

    /* ADC initialize */
    adc_init ();
}

/* Update read data from sharps. */
void
sharp_update (uint8_t sharp_mask)
{
    uint8_t compt;

    /* Go through the bit mask */
    for (compt = 0; compt < SHARP_NUMBER; compt++)
      {
	/* Check if the bit/sharp id is set */
	if (bit_is_set (sharp_mask, compt))
	  {
	    /* Start the capture */
	    adc_start (compt + 1);
	    /* Wait until ADC mesure is finished */
	    while (!adc_checkf ())
		;
	    /* Store the value */
	    sharp_values_[compt] = adc_read ();
	  }
      }
}

/* Get raw cached data from sharps. */
uint16_t
sharp_get_raw (uint8_t sharp_id)
{
    /* Sanity check */
    if (sharp_id < SHARP_NUMBER)
	/* Return raw cached value */
	return sharp_values_[sharp_id];
    return 0;
}

/* Configure the thresholds of a sharp. */
void
sharp_set_threshold (uint8_t sharp_id, uint16_t low, uint16_t high)
{
    /* Sanity check */
    if (sharp_id < SHARP_NUMBER)
      {
	/* Set low and high threshold */
	sharp_threshold[sharp_id][0] = low;
	sharp_threshold[sharp_id][1] = high;
      }
}

/* Get interpreted value from sharps. */
uint8_t
sharp_get_interpreted (uint8_t sharp_id)
{
    uint8_t current_state;

    /* Sanity check */
    if (sharp_id < SHARP_NUMBER)
      {
	/* Check if sharp state is lower than the low threshold */
	if (sharp_values_[sharp_id] < sharp_threshold[sharp_id][0])
	    /* Update current state seen by the sharp */
	    current_state = 0;
	/* Check if sharp state is higher than the high threshold */
	else if (sharp_values_[sharp_id] > sharp_threshold[sharp_id][1])
	    /* Update current state seen by the sharp */
	    current_state = 1;
	/* In the middle */
	else
	    /* Return the previous value */
	    return sharp_previous_values_[sharp_id];

	/* Filter the value */
	/* Check if previous value is the current state */
	if (sharp_previous_values_[sharp_id] == current_state)
	  {
	    /* Reset filter counter */
	    sharp_filter_[sharp_id] = SHARP_FILTER_NUMBER;
	    /* Return current state */
	    return current_state;
	  }
	/* Otherwise, check if this sharp value has been the same
	 * SHARP_FILTER_NUMBER times */
	else if (sharp_filter_[sharp_id]-- == 0)
	  {
	    /* Current value change (for SHARP_FILTER_NUMBER times)! */
	    /* Update previous value */
	    sharp_previous_values_[sharp_id] = current_state;
	    /* Reset filter counter */
	    sharp_filter_[sharp_id] = SHARP_FILTER_NUMBER;
	    /* Return current state */
	    return current_state;
	  }
	else
	    /* Return previous state */
	    return sharp_previous_values_[sharp_id];
      }
    /* Error */
    return 2;
}
