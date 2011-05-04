#ifndef fsm_queue_h
#define fsm_queue_h
/* fsm_queue.h */
/* ai - Robot Artificial Intelligence. {{{
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

/** Post an event to be processed in the next main loop. */
void
fsm_queue_post_event (uint8_t event);

/** Poll for event in the event queue (return non zero if there is an event
 * pending). */
uint8_t
fsm_queue_poll (void);

/** Pop one event from the event queue. */
uint8_t
fsm_queue_pop_event (void);

#endif /* fsm_queue_h */
