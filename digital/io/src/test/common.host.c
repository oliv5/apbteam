/* common.c */
/*  {{{
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
#include "../giboulee.h"

#include <stdio.h>

static uint16_t asserv_arm_position = 0;

/* Yerk export */
enum team_color_e bot_color = RED_TEAM;

/* Define functions for debug */
void
trap_setup_path_to_box (uint8_t box_id)
{
    printf ("[trap] Configure trap doors to open %d.\n", box_id);
}

void
trap_close_rear_panel (void)
{
    printf ("[trap] Closing rear panel.\n");
}

void
trap_open_rear_panel (void)
{
    printf ("[trap] Opening rear panel.\n");
}

void
asserv_move_linearly (int32_t distance)
{
    printf ("[asserv] Make the bot move linearly of %d mm.\n", distance);
}

void
asserv_move_arm (uint16_t position, uint8_t speed)
{
    asserv_arm_position += position;
    printf ("[asserv] Move arm at %d (speed: %d).\n",
	    asserv_arm_position, speed);
}

void
asserv_close_input_hole (void)
{
    printf ("[asserv] Put the arm in front of the input hole.\n");
    asserv_move_arm (asserv_arm_position %
		     BOT_ARM_THIRD_ROUND, BOT_ARM_SPEED);
}

uint16_t
asserv_get_arm_position (void)
{
    return asserv_arm_position;
}

void
asserv_arm_set_position_reached (uint16_t position)
{
    printf ("[asserv] Arm notifier at position %d (we are at %d).\n",
	    position, asserv_arm_position);
}

void
asserv_go_to_distributor (void)
{
    printf ("[asserv] Go to distributor.\n");
}

void
asserv_get_position (void *undef)
{
    printf ("[asserv] Asking position of the bot.\n");
}

void
asserv_goto (uint32_t x, uint32_t y)
{
    printf ("[asserv] Move the bot to (%d; %d).\n", x, y);
}

void
asserv_goto_angle (int16_t angle)
{
    printf ("[asserv] Move the bot to face %X.\n", angle);
}

void
asserv_set_x_position (int32_t x)
{
    printf ("[asserv] Set X position to %d.\n", x);
}

void
asserv_set_y_position (int32_t y)
{
    printf ("[asserv] Set Y position to %d.\n", y);
}

void
asserv_set_angle_position (int16_t a)
{
    printf ("[asserv] Set angle position to %d.\n", a);
}

void
asserv_set_position (int32_t x, int32_t y, int16_t a)
{
    printf ("[asserv] Set bot position to (%d, %d, %d).\n", x, y, a);
}

void
asserv_go_to_the_wall (void)
{
    printf ("[asserv] Go to the wall.\n");
}

void
chrono_init (void)
{
    printf ("[chrono] Chrono init.\n");
}

void
gutter_start (void)
{
    printf ("[FSM:gutter] Start the gutter FSM\n");
}
