/* fsm_queue.c */
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
#include "common.h"
#include "fsm_queue.h"

/** Maximum number of events in post queue. */
#define FSM_QUEUE_SIZE 8

/** Module context. */
struct fsm_queue_t
{
    /** Events to post to the FSM in next iteration. */
    uint8_t events[FSM_QUEUE_SIZE];
    /** First event in the queue. */
    uint8_t head;
    /** Number of events in the queue. */
    uint8_t nb;
};

/** Global context. */
static struct fsm_queue_t fsm_queue_global;
# define ctx fsm_queue_global

void
fsm_queue_post_event (uint8_t event)
{
    assert (ctx.nb < FSM_QUEUE_SIZE);
    uint8_t tail = (ctx.head + ctx.nb) % FSM_QUEUE_SIZE;
    ctx.events[tail] = event;
    ctx.nb++;
}

uint8_t
fsm_queue_poll (void)
{
    return ctx.nb;
}

uint8_t
fsm_queue_pop_event (void)
{
    uint8_t e;
    assert (ctx.nb > 0);
    e = ctx.events[ctx.head];
    ctx.nb--;
    ctx.head = (ctx.head + 1) % FSM_QUEUE_SIZE;
    return e;
}

