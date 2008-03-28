#ifndef sharp_h
#define sharp_h
/* sharp.h */
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
 * @file Module to control distance measuring sensors (sharp).
 * They can be used to detect obstacles, for example a bot.
 * It uses the Analog to Digital Converter (ADC) of the AVR to read and
 * understand data from the sharp sensors.
 * This module keep a value of the last read values from the sharp and update
 * it when calling the correct function.
 */

/**
 * Number of sharps.
 */
#define SHARP_NUMBER 6

/**
 * Front left sharp.
 */
#define SHARP_FRONT_LEFT _BV(0)

/**
 * Front right sharp.
 */
#define SHARP_FRONT_RIGHT _BV(1)

/**
 * Back left sharp.
 */
#define SHARP_BACK_LEFT _BV(2)

/**
 * Back right sharp.
 */
#define SHARP_BACK_RIGHT _BV(3)

/**
 * Low (0 index) and high (1 index) thresholds for interpreted sharp values.
 */
extern uint16_t sharp_threshold[SHARP_NUMBER][2];

/**
 * Initialize sharp module.
 */
void sharp_init (void);

/**
 * Update read data from sharps.
 * This function is blocking. To get the value you have to use the get
 * function (@a sharp_get_raw).
 * @param sharp_mask list of sharps (using a mask) to update.
 */
void sharp_update (uint8_t sharp_mask);

/**
 * Get raw cached data from sharps.
 * It read the cached data (updated by the @a sharp_update function).
 * @param sharp_id the sharp id to get raw data from.
 * @return the raw data read from the sharps (0 is a non valid value).
 */
uint16_t sharp_get_raw (uint8_t sharp_id);

/**
 * Configure the thresholds of a sharp.
 * @param sharp_id the sharp id to configure the thresholds.
 * @param low low threshold of the sharp.
 * @param high high threshold of the sharp.
 */
void sharp_set_threshold (uint8_t sharp_id, uint16_t low, uint16_t high);

/**
 * Get interpreted value from sharps.
 * It is used to know if there is something in front of the sharp.
 * @param sharp_id the sharp number to get interpreted value from.
 * @return
 *  - 0 if there is nothing in front of the sharp.
 *  - 1 if there is something in front of the sharp;
 *  - other values when error occurs.
 */
uint8_t sharp_get_interpreted (uint8_t sharp_id);

#endif /* sharp_h */
