#ifndef gutter_h
#define gutter_h
/* gutter.h */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2008 Nélio Laranjeiro
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
 * Start the gutter FSM.
 */
void
gutter_start (void);

/**
 * Do we need to generate a wait_finished event?
 * You need to call this function in the main loop in order to ensure we
 * generate a wait_finished event when the gutter FSM need one.
 * The purpose is to let the gutter FSM wait a specific number of cycle it
 * wants.
 * @return
 *   - 0 if you do need to generate a wait_finished event
 *   - 1 if you need to generate a wait_finished event for the gutter FSM.
 */
uint8_t
gutter_generate_wait_finished_event (void);

#endif /* gutter_h */
