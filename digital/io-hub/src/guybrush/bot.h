#ifndef bot_h
#define bot_h
/* bot.h */
/* guybrush - Eurobot 2012 AI. {{{
 *
 * Copyright (C) 2012 Nicolas Schodet
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

/** Robot specific defines. */

/** Scaling factor, millimeter per step. */
#ifdef HOST
# define BOT_SCALE 0.0395840674352314
#else
# define BOT_SCALE 0.0317975134344
#endif

/** Distance from the robot axis to the front. */
#define BOT_SIZE_FRONT 165
/** Distance from the robot axis to the lower clamp back. */
#define BOT_SIZE_LOWER_CLAMP_FRONT 100
/** Distance from the robot axis to the back. */
#define BOT_SIZE_BACK 134
/** Distance from the robot axis to the side. */
#define BOT_SIZE_SIDE 172
/** Maximum distance from the robot base center to one of its edge. */
#define BOT_SIZE_RADIUS 230
/** Maximum distance from the robot base center to one of its edge, at the
 * back. */
#define BOT_SIZE_BACK_RADIUS 180

/** Distance between the front contact point and the robot center. */
#define BOT_BACK_CONTACT_DIST_MM BOT_SIZE_BACK
/** Angle error at the front contact point. */
#define BOT_BACK_CONTACT_ANGLE_ERROR_DEG 0

/** Speed used for initialisation. */
#ifdef HOST
# define BOT_SPEED_INIT 0x20, 0x20, 0x20, 0x20
#else
# define BOT_SPEED_INIT 0x10, 0x10, 0x10, 0x10
#endif
/** Normal cruise speed. */
#define BOT_SPEED_NORMAL 0x50, 0x60, 0x20, 0x20
/** Approach speed. */
#define BOT_SPEED_APPROACH 0x20, 0x30, 0x20, 0x20

#endif /* bot_h */
