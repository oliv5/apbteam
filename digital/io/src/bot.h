#ifndef bot_h
#define bot_h
/* bot.h */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2010 Nicolas Schodet
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
 * Specific defines about the robot dimensions and competion rules.
 */

/**
 * Duration of a match in milliseconds.
 */
#define MATCH_DURATION_MS 90000

/**
 * The scaling factor, millimeter per step.
 */
#ifdef HOST
# define BOT_SCALE 0.0395840674352314
#else
# define BOT_SCALE 0.0413530725332892
#endif

/**
 * The size of the bot.
 */
#define BOT_LENGTH 300
#define BOT_WIDTH 310

/**
 * Distance required to be away from a border to be able to turn freely.
 * In millimeter.
 */
#define BOT_MIN_DISTANCE_TURN_FREE 300

/**
 * Definition of the colors.
 */
enum team_color_e
{
    RED_TEAM = 0,
    BLUE_TEAM = 1
};

/**
 * Our color.
 */
extern enum team_color_e bot_color;

#endif /* bot_h */
