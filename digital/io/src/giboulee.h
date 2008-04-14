#ifndef giboulee_h
#define giboulee_h
// giboulee.h
// io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
//
// Copyright (C) 2008 Dufour Jérémy
//
// Robot APB Team/Efrei 2004.
//        Web: http://assos.efrei.fr/robot/
//      Email: robot AT efrei DOT fr
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// }}}

/**
 * @file Some defines for the bot itself.
 * In this header file, you can find some configuration value for the length
 * of a step when the bot is moving for example.
 */

/**
 * The speed of the arm.
 * It is expressed in number of steps by asserv board cycle (4.44ms).
 */
#define BOT_ARM_SPEED 3

/**
 * The number of step of a whole rotation.
 */
#define BOT_ARM_STEP_ROUND 5000

/**
 * Number of step to make a third of a round.
 */
#define BOT_ARM_THIRD_ROUND BOT_ARM_STEP_ROUND / 3

/**
 * The number of steps to do to open the input hole when the arm is closing
 * it.
 * For the moment, 15% of a complete turn
 */
#define BOT_ARM_MIN_TO_OPEN (BOT_ARM_STEP_ROUND / 100 * 15)

/**
 * Steps number relative to the beginning of a third round for the 'noted'
 * position required by the get sample FSM.
 */
#define BOT_ARM_NOTED_POSITION (BOT_ARM_THIRD_ROUND / 3 * 2)

/**
 * How to compute a angle for giboulee?
 * One degree is 65536 / 360
 */
#define BOT_ANGLE_DEGREE (65536 / 360)

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

#endif // giboulee_h
