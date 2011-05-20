#ifndef bot_h
#define bot_h
/* bot.h */
/* robospierre - Eurobot 2011 AI. {{{
 *
 * Copyright (C) 2011 Nicolas Schodet
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
# define BOT_SCALE 0.0415178942124
#endif

/** Distance between the front contact point and the robot center. */
#define BOT_FRONT_CONTACT_DIST_MM 150
/** Angle error at the front contact point. */
#define BOT_FRONT_CONTACT_ANGLE_ERROR_DEG 0

/** Speed used for initialisation. */
#define BOT_SPEED_INIT 0x10, 0x10, 0x10, 0x10
/** Normal cruise speed. */
#define BOT_SPEED_NORMAL 0x40, 0x40, 0x20, 0x20

#ifdef HOST

# define BOT_CLAMP_SLOT_FRONT_BOTTOM_ELEVATION_STEP 0
# define BOT_CLAMP_SLOT_FRONT_MIDDLE_ELEVATION_STEP (0x3b0b / 2)
# define BOT_CLAMP_SLOT_FRONT_TOP_ELEVATION_STEP 0x3b0b
# define BOT_CLAMP_SLOT_BACK_BOTTOM_ELEVATION_STEP 0
# define BOT_CLAMP_SLOT_BACK_MIDDLE_ELEVATION_STEP (0x3b0b / 2)
# define BOT_CLAMP_SLOT_BACK_TOP_ELEVATION_STEP 0x3b0b
# define BOT_CLAMP_SLOT_SIDE_ELEVATION_STEP 0x3b0b
# define BOT_CLAMP_BAY_FRONT_LEAVE_ELEVATION_STEP (0x3b0b / 3)
# define BOT_CLAMP_BAY_BACK_LEAVE_ELEVATION_STEP (0x3b0b / 3)
# define BOT_CLAMP_BAY_SIDE_ENTER_LEAVE_ELEVATION_STEP (0x3b0b / 2)

# define BOT_CLAMP_BAY_FRONT_ROTATION_STEP 0
# define BOT_CLAMP_BAY_BACK_ROTATION_STEP 0x11c6
# define BOT_CLAMP_BAY_SIDE_ROTATION_STEP (0x11c6 / 2)

#else /* !HOST */

# define BOT_CLAMP_SLOT_FRONT_BOTTOM_ELEVATION_STEP 0
# define BOT_CLAMP_SLOT_FRONT_MIDDLE_ELEVATION_STEP 0x1d83
# define BOT_CLAMP_SLOT_FRONT_TOP_ELEVATION_STEP 0x3288
# define BOT_CLAMP_SLOT_BACK_BOTTOM_ELEVATION_STEP 0
# define BOT_CLAMP_SLOT_BACK_MIDDLE_ELEVATION_STEP 0x1d83
# define BOT_CLAMP_SLOT_BACK_TOP_ELEVATION_STEP 0x3288
# define BOT_CLAMP_SLOT_SIDE_ELEVATION_STEP 0x3288
# define BOT_CLAMP_BAY_FRONT_LEAVE_ELEVATION_STEP 0x1d83
# define BOT_CLAMP_BAY_BACK_LEAVE_ELEVATION_STEP 0x1d83
# define BOT_CLAMP_BAY_SIDE_ENTER_LEAVE_ELEVATION_STEP 0x1d83

# define BOT_CLAMP_BAY_FRONT_ROTATION_STEP 0
# define BOT_CLAMP_BAY_BACK_ROTATION_STEP 0x10e2
# define BOT_CLAMP_BAY_SIDE_ROTATION_STEP 0x820

#endif /* !HOST */

#define BOT_CLAMP_ELEVATION_SPEED 0x60
#define BOT_CLAMP_ROTATION_SPEED 0x30

#define BOT_PWM_CLAMP 2
#define BOT_PWM_DOOR_FRONT_BOTTOM 0
#define BOT_PWM_DOOR_FRONT_TOP 1
#define BOT_PWM_DOOR_BACK_BOTTOM 3
#define BOT_PWM_DOOR_BACK_TOP 4

#define BOT_PWM_CLAMP_OPEN_TIME 225
#define BOT_PWM_CLAMP_OPEN 0x3ff, 225, 0
#define BOT_PWM_CLAMP_CLOSE_TIME 225
#define BOT_PWM_CLAMP_CLOSE -0x3ff, 225, 0

#define BOT_PWM_DOOR_OPEN_TIME 225
#define BOT_PWM_DOOR_OPEN 0x3ff, 225, 0
#define BOT_PWM_DOOR_CLOSE_TIME 225
#define BOT_PWM_DOOR_CLOSE -0x3ff, 225, 0

#endif /* bot_h */
