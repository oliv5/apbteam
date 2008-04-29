#ifndef main_h
#define main_h
/* main.h */
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
 * Post a event to the top FSM in the next iteration of main loop.
 * You just need to set variable to the value of the event you want to post to
 * the top FSM and add one to it (because some event could have a zero value).
 * It will be posted and cleared in the next main loop iteration.
 */
extern uint8_t main_post_event_for_top_fsm;

/**
 * Do not generate sharps event for FSM during a certain count of cycles.
 * This flag is used by the move FSM to prevent it from being alerted by the
 * main loop that there is an object in from of the bot.
 */
extern uint16_t main_sharp_ignore_event;

#endif /* main_h */
