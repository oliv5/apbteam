/* logistic.c */
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
#include "common.h"
#include "logistic.h"

#include "clamp.h"
#include "defs.h"

#include "debug.host.h"

/** Handle elements stored inside the robot. */

/** Global context. */
struct logistic_t logistic_global;
#define ctx logistic_global

inline void
logistic_debug_dump (void)
{
#ifdef HOST
    uint8_t i;
    static const char *names[][CLAMP_SLOT_NB] = {
	  { "f1", "f2", "f3", "b1", "b2", "b3", "s1" },
	  { "F1", "F2", "F3", "B1", "B2", "B3", "S1" }
    };
    static const char *names_dir[] = { "--", "<-", "->" };
    DPRINTF ("%s", names_dir[ctx.collect_direction]);
    for (i = 0; i < CLAMP_SLOT_NB; i++)
      {
	DPRINTF (" %s", ctx.slots[i]
		 ? names[ELEMENT_IS_HEAD (ctx.slots[i]) ? 1 : 0][i]
		 : "__");
      }
    if (ctx.moving_from != CLAMP_SLOT_NB)
      {
	DPRINTF ("    %s => %s", names[0][ctx.moving_from], names[0][ctx.moving_to]);
      }
    DPRINTF ("\n");
#endif
}

/** Examine current state and take a decision. */
static void
logistic_decision (void)
{
    uint8_t i;
    /* If currently moving, do not take decision. */
    if (ctx.moving_from != CLAMP_SLOT_NB)
	return;
    /* Determine collect_direction. */
    uint8_t front_head = 0, back_head = 0,
	    front_element = 0, back_element = 0;
    uint8_t collect_direction;
    for (i = CLAMP_SLOT_FRONT_BOTTOM; i <= CLAMP_SLOT_FRONT_TOP; i++)
      {
	if (ctx.slots[i])
	  {
	    front_element++;
	    if (ELEMENT_IS_HEAD (ctx.slots[i]))
		front_head++;
	  }
      }
    for (i = CLAMP_SLOT_BACK_BOTTOM; i <= CLAMP_SLOT_BACK_TOP; i++)
      {
	if (ctx.slots[i])
	  {
	    back_element++;
	    if (ELEMENT_IS_HEAD (ctx.slots[i]))
		back_head++;
	  }
      }
    if (front_head < back_head)
	collect_direction = DIRECTION_FORWARD;
    else if (front_head > back_head)
	collect_direction = DIRECTION_BACKWARD;
    else if (front_head)
      {
	if (front_element < back_element)
	    collect_direction = DIRECTION_FORWARD;
	else if (front_element > back_element)
	    collect_direction = DIRECTION_BACKWARD;
	else
	    collect_direction = ctx.collect_direction;
      }
    else
	collect_direction = ctx.collect_direction;
    ctx.collect_direction = collect_direction;
    /* Now use this direction. */
    uint8_t collect_bay, storage_bay;
    uint8_t collect_bay_head, storage_bay_head;
    if (collect_direction == DIRECTION_FORWARD)
      {
	collect_bay = CLAMP_SLOT_FRONT_BOTTOM;
	storage_bay = CLAMP_SLOT_BACK_BOTTOM;
	collect_bay_head = front_head;
	storage_bay_head = back_head;
	ctx.clamp_pos_idle = CLAMP_SLOT_FRONT_MIDDLE;
      }
    else
      {
	collect_bay = CLAMP_SLOT_BACK_BOTTOM;
	storage_bay = CLAMP_SLOT_FRONT_BOTTOM;
	collect_bay_head = back_head;
	storage_bay_head = front_head;
	ctx.clamp_pos_idle = CLAMP_SLOT_BACK_MIDDLE;
      }
    /* Find a destination for an element move. */
    uint8_t moving_to = CLAMP_SLOT_NB;
    uint8_t moving_from = CLAMP_SLOT_NB;
    if (!ctx.slots[collect_bay + 1])
      {
	/* Movements in collect bay possible. */
	if (ELEMENT_IS_HEAD (ctx.slots[collect_bay + 0]))
	  {
	    moving_to = collect_bay + 2;
	    moving_from = collect_bay + 0;
	  }
      }
    if (moving_to == CLAMP_SLOT_NB && !ctx.slots[storage_bay + 1])
      {
	/* No movement yet and movements in storage bay possible. */
	if (ELEMENT_IS_HEAD (ctx.slots[storage_bay + 0]))
	  {
	    moving_to = storage_bay + 2;
	    moving_from = storage_bay + 0;
	  }
	else if (storage_bay_head)
	  {
	    if (!ctx.slots[storage_bay + 0])
		moving_to = storage_bay + 0;
	    else if (!ctx.slots[storage_bay + 1])
		moving_to = storage_bay + 1;
	  }
      }
    if (moving_to == CLAMP_SLOT_NB && !ctx.slots[CLAMP_SLOT_SIDE])
      {
	/* No movement yet, store in side slot. */
	moving_to = CLAMP_SLOT_SIDE;
      }
    /* Find a source if available. */
    if (moving_to != CLAMP_SLOT_NB && moving_from == CLAMP_SLOT_NB)
      {
	if (ctx.slots[collect_bay + 0])
	    moving_from = collect_bay + 0;
	else if (ctx.slots[CLAMP_SLOT_SIDE])
	    moving_from = CLAMP_SLOT_SIDE;
      }
    /* Ask for movement. */
    if (moving_from != CLAMP_SLOT_NB)
      {
	ctx.moving_from = moving_from;
	ctx.moving_to = moving_to;
      }
    logistic_debug_dump ();
}

void
logistic_init (void)
{
    uint8_t i;
    for (i = 0; i < CLAMP_SLOT_NB; i++)
	ctx.slots[i] = 0;
    ctx.moving_from = ctx.moving_to = CLAMP_SLOT_NB;
    ctx.collect_direction = DIRECTION_FORWARD;
}

void
logistic_update (void)
{
}

void
logistic_element_new (uint8_t pos, uint8_t element)
{
    assert (pos < CLAMP_SLOT_NB);
    assert (!ctx.slots[pos]);
    ctx.slots[pos] = element;
    logistic_decision ();
}

void
logistic_element_move_done (void)
{
    assert (!ctx.slots[ctx.moving_to]);
    ctx.slots[ctx.moving_to] = ctx.slots[ctx.moving_from];
    ctx.slots[ctx.moving_from] = 0;
    ctx.moving_from = ctx.moving_to = CLAMP_SLOT_NB;
    logistic_decision ();
}

