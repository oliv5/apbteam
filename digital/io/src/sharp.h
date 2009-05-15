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
 * List of sharps supported (and their identifier).
 * Making a change to this list required an update to the EEPROM module key.
 */
typedef enum sharp_list
{
    /**
     * Front left sharp.
     */
    SHARP_FRONT_LEFT = 0,
    /**
     * Front middle sharp.
     */
    SHARP_FRONT_MIDDLE = 1,
    /**
     * Front right sharp.
     */
    SHARP_FRONT_RIGHT = 2,
    /**
     * Back left sharp.
     */
    SHARP_BACK_LEFT = 3,
    /**
     * Back right sharp.
     */
    SHARP_BACK_RIGHT = 4,
    /**
     * Middle back sharp. It used to know if there are some pucks in the back
     * of the bot.
     */
    SHARP_BACK_MIDDLE = 5,
    /**
     * Number of sharps.
     * It must be the last member.
     */
    SHARP_NUMBER
} sharp_list;

/** Sharps conversion table number of elements. */
#define SHARP_NB_ELEMENT_TABLE_CONV 12

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
 */
void sharp_update (void);

/**
 * Get raw cached data from sharps.
 * It read the cached data (updated by the @a sharp_update function).
 * @param sharp_id the sharp id to get raw data from.
 * @return the raw data read from the sharps (0 is a non valid value).
 */
uint16_t sharp_get_raw (uint8_t sharp_id);

/** 
 * Get the distance of the sharp computed on the value. It does a search in the
 * table.
 * @param  sharp_value  the value of the seen by the sharp.
 * @return  the value in mm of the object seen.
 */
uint16_t
sharp_get_distance_mm (uint16_t sharp_value);

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

/**
 * Is there an obstacle in front of the bot?
 * This function correctly handles the moving direction of the bot to check
 * only the necessary sharps.
 * @param moving_direction
 *   - 1 when moving forward ;
 *   - 2 when moving backward.
 * @return
 *   - 0 if there is nothing in front ;
 *   - 1 if there is an obstacle in front of the bot.
 */
uint8_t
sharp_path_obstrued (uint8_t moving_direction);

#endif /* sharp_h */
