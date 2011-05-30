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
    DPRINTF ("construct_possible: %u\n", ctx.construct_possible);
    DPRINTF ("prepare: %u\n", ctx.prepare);
    DPRINTF ("ready: %u\n", ctx.ready);
    DPRINTF ("need_prepare: %u\n", ctx.need_prepare);
#endif
}

static void
logistic_tower_possible ()
{
    uint8_t i, head = 0, pawn = 0;
    ctx.ready = 0;
    /* If clamp broken. */
    if (ctx.prepare == 3 && ctx.slots[CLAMP_SLOT_FRONT_BOTTOM])
      {
	ctx.construct_possible = 2;
	ctx.ready = 1;
	ctx.collect_direction = DIRECTION_BACKWARD;
	return;
      }
    else if (ctx.prepare == 3 && ctx.slots[CLAMP_SLOT_BACK_BOTTOM])
      {
	ctx.construct_possible = 2;
	ctx.ready = 1;
	ctx.collect_direction = DIRECTION_FORWARD;
	return;
      }

    for (i = CLAMP_SLOT_FRONT_BOTTOM; i <= CLAMP_SLOT_NB; i++)
      {
	    if (ELEMENT_IS_HEAD (ctx.slots[i]))
		head++;
	    else if (ctx.slots[i])
		pawn++;
	    if (head >= 1 && pawn >= 2)
	    {
	      ctx.construct_possible = 1;
	      return;
	    }
      }
    if (pawn || head)
	ctx.construct_possible = 2;
    else
	ctx.construct_possible = 0;
    return;
}

/* Check if we can't take element anymore and we need to be authorized to
 * unload. */
static void
logistic_check_need_prepare ()
{
    uint8_t i, head = 0, pawn = 0;
    for (i = CLAMP_SLOT_FRONT_BOTTOM; i < CLAMP_SLOT_NB; i++)
      {
	    if (ELEMENT_IS_HEAD (ctx.slots[i]))
		head++;
	    else if (ctx.slots[i])
		pawn++;
      }
    if ((head == 1 && pawn == 3) ||
	head == 2 ||
	pawn > 4)
	ctx.need_prepare = 1;
    else
	ctx.need_prepare = 0;
}

/** Build something not a tower. */
static void
logistic_put_element ()
{
    uint8_t dir_bay, nodir_bay;
    if (ctx.collect_direction == DIRECTION_FORWARD)
      {
	dir_bay = CLAMP_SLOT_FRONT_BOTTOM;
	nodir_bay = CLAMP_SLOT_BACK_BOTTOM;
      }
    else
      {
	dir_bay = CLAMP_SLOT_BACK_BOTTOM;
	nodir_bay = CLAMP_SLOT_FRONT_BOTTOM;
      }

    /* If we may build a little tower ? */
    /* Put the tower on the pawn. */
    if (ELEMENT_IS_HEAD (ctx.slots[nodir_bay + 2]) &&
	ctx.slots[nodir_bay] &&
	!ctx.slots[nodir_bay + 1])
      {
	ctx.moving_from = nodir_bay + 2;
	ctx.moving_to = nodir_bay + 1;
	return;
      }
    /* Little tower is ready. */
    else if (ELEMENT_IS_HEAD (ctx.slots[nodir_bay + 1]) &&
	     ctx.slots[nodir_bay])
	ctx.ready = 1;
    else if (ELEMENT_IS_HEAD (ctx.slots[nodir_bay]))
	ctx.ready = 1;
    /* Find a pawn for a lonely head. */
    else if (ELEMENT_IS_HEAD (ctx.slots[nodir_bay + 2]))
      {
	if (!ctx.slots[nodir_bay] && ctx.slots[CLAMP_SLOT_SIDE])
	  {
	    ctx.moving_from = CLAMP_SLOT_SIDE;
	    ctx.moving_to = nodir_bay;
	  }
	else if (!ctx.slots[nodir_bay] && !ELEMENT_IS_HEAD (ctx.slots[dir_bay + 2]) &&
		 ctx.slots[dir_bay + 2])
	  {
	    ctx.moving_from = dir_bay + 2;
	    ctx.moving_to = nodir_bay;
	  }
	else if (!ctx.slots[nodir_bay] &&
		 !ELEMENT_IS_HEAD (ctx.slots[dir_bay + 1]) &&
		 ctx.slots[dir_bay + 1])
	  {
	    ctx.moving_from = dir_bay + 1;
	    ctx.moving_to = nodir_bay;
	  }
	/* No pawn ! */
	else if (!ctx.slots[nodir_bay])
	  {
	    ctx.moving_from = nodir_bay + 2;
	    ctx.moving_to = nodir_bay;
	  }
      }
    /* If we may build a little tower ? */
    /* Put the tower on the pawn. */
    else if (ELEMENT_IS_HEAD (ctx.slots[dir_bay + 2]) &&
	ctx.slots[dir_bay] &&
	!ctx.slots[dir_bay + 1])
      {
	ctx.moving_from = dir_bay + 2;
	ctx.moving_to = dir_bay + 1;
	return;
      }
    /* Little tower is ready. */
    else if (ELEMENT_IS_HEAD (ctx.slots[dir_bay + 1]) &&
	     ctx.slots[dir_bay])
	ctx.ready = 1;
    else if (ELEMENT_IS_HEAD (ctx.slots[dir_bay]))
	ctx.ready = 1;
    /* Find a pawn for a lonely head. */
    else if (ELEMENT_IS_HEAD (ctx.slots[dir_bay + 2]))
      {
	if (!ctx.slots[dir_bay] && ctx.slots[CLAMP_SLOT_SIDE])
	  {
	    ctx.moving_from = CLAMP_SLOT_SIDE;
	    ctx.moving_to = dir_bay;
	  }
	else if (!ctx.slots[dir_bay] &&
		 !ELEMENT_IS_HEAD (ctx.slots[nodir_bay + 2]) &&
		 ctx.slots[nodir_bay + 2])
	  {
	    ctx.moving_from = nodir_bay + 2;
	    ctx.moving_to = dir_bay;
	  }
	else if (!ctx.slots[dir_bay] &&
		 !ELEMENT_IS_HEAD (ctx.slots[nodir_bay + 1]) &&
		 ctx.slots[nodir_bay + 1])
	  {
	    ctx.moving_from = nodir_bay + 1;
	    ctx.moving_to = dir_bay;
	  }
	/* No pawn ! */
	else if (!ctx.slots[dir_bay])
	  {
	    ctx.moving_from = dir_bay + 2;
	    ctx.moving_to = dir_bay;
	  }
      }
    /* No head, find any pawn. */
    else if (!ctx.slots[nodir_bay])
      {
	   ctx.moving_to = nodir_bay;
	   if (ctx.slots[nodir_bay + 2])
	       ctx.moving_from = nodir_bay + 2;
	   else if (ctx.slots[CLAMP_SLOT_SIDE])
	       ctx.moving_from = CLAMP_SLOT_SIDE;
	   else if (ctx.slots[dir_bay + 2])
	       ctx.moving_from = dir_bay + 2;
	   else if (ctx.slots[dir_bay])
	       ctx.moving_from = dir_bay;
      }
    else if (ctx.slots[nodir_bay])
      {
	ctx.ready = 1;
      }
}

/** If we picked up a tower. */
static void
logistic_new_tower ()
{
    uint8_t dir_bay, nodir_bay;
    if (ctx.collect_direction == DIRECTION_FORWARD)
      {
	dir_bay = CLAMP_SLOT_FRONT_BOTTOM;
	nodir_bay = CLAMP_SLOT_BACK_BOTTOM;
      }
    else
      {
	dir_bay = CLAMP_SLOT_BACK_BOTTOM;
	nodir_bay = CLAMP_SLOT_FRONT_BOTTOM;
      }
    if (ctx.slots[dir_bay] == ELEMENT_TOWER)
      {
	ctx.ready = 1;
	if (ctx.collect_direction == DIRECTION_FORWARD)
	    ctx.collect_direction = DIRECTION_BACKWARD;
	else
	    ctx.collect_direction = DIRECTION_FORWARD;
      }
    if (ctx.slots[nodir_bay] == ELEMENT_TOWER)
	ctx.ready = 1;
}

/** Examine current state and take a decision. */
static void
logistic_decision (void)
{
    /* Reset. */
    ctx.moving_from = CLAMP_SLOT_NB;
    ctx.moving_to = CLAMP_SLOT_NB;
    ctx.construct_possible = 0;
    ctx.ready = 0;
    ctx.need_prepare = 0;

    /* Update if a something is possible. */
    logistic_tower_possible ();

    /* Clamp is broken, can't move anything. */
    if (ctx.prepare == 3)
	return;

    /* We founded a tower ! */
    logistic_new_tower ();
    if (ctx.ready)
	return;

    /* Check if we really need to prepare something. */
    logistic_check_need_prepare ();

    /* A head is here, put it in the top ! */
    if (ctx.prepare != 2)
      {
	if (ELEMENT_IS_HEAD (ctx.slots[CLAMP_SLOT_FRONT_BOTTOM]) &&
	    !ctx.slots[CLAMP_SLOT_FRONT_TOP])
	  {
	    ctx.moving_from = CLAMP_SLOT_FRONT_BOTTOM;
	    ctx.moving_to = CLAMP_SLOT_FRONT_TOP;
	    if (!ctx.slots[CLAMP_SLOT_BACK_TOP])
	      {
		ctx.collect_direction = DIRECTION_BACKWARD;
		ctx.clamp_pos_idle = CLAMP_SLOT_BACK_MIDDLE;
	      }
	    return;
	  }
	if (ELEMENT_IS_HEAD (ctx.slots[CLAMP_SLOT_BACK_BOTTOM]) &&
	    !ctx.slots[CLAMP_SLOT_BACK_TOP])
	  {
	    ctx.moving_from = CLAMP_SLOT_BACK_BOTTOM;
	    ctx.moving_to = CLAMP_SLOT_BACK_TOP;
	    if (!ctx.slots[CLAMP_SLOT_FRONT_TOP])
	      {
		ctx.collect_direction = DIRECTION_FORWARD;
		ctx.clamp_pos_idle = CLAMP_SLOT_FRONT_MIDDLE;
	      }
	    return;
	  }
      }
    /* We can build a tower and we are authorized to do so. */
    if (ctx.construct_possible == 1 && ctx.prepare)
      {
	/* Where to build the tower. */
	uint8_t build_dir;
	if (ELEMENT_IS_HEAD (ctx.slots[CLAMP_SLOT_BACK_TOP]) &&
	    ELEMENT_IS_HEAD (ctx.slots[CLAMP_SLOT_FRONT_TOP]))
	  {
	    if (ctx.slots[CLAMP_SLOT_BACK_BOTTOM] &&
		ctx.slots[CLAMP_SLOT_FRONT_BOTTOM])
		build_dir = ctx.collect_direction;
	    else if (ctx.slots[CLAMP_SLOT_BACK_BOTTOM])
		build_dir = DIRECTION_BACKWARD;
	    else
		build_dir = DIRECTION_FORWARD;
	  }
	else if (ELEMENT_IS_HEAD (ctx.slots[CLAMP_SLOT_BACK_TOP]))
	    build_dir = DIRECTION_BACKWARD;
	else
	    build_dir = DIRECTION_FORWARD;
	/* Adapt collect direction. */
	if (build_dir == DIRECTION_FORWARD)
	    ctx.collect_direction = DIRECTION_BACKWARD;
	else
	    ctx.collect_direction = DIRECTION_FORWARD;
	/* Fill with pawns. */
	uint8_t build_bay, collect_bay;
	if (build_dir == DIRECTION_FORWARD)
	  {
	    build_bay = CLAMP_SLOT_FRONT_BOTTOM;
	    collect_bay = CLAMP_SLOT_BACK_BOTTOM;
	  }
	else
	  {
	    build_bay = CLAMP_SLOT_BACK_BOTTOM;
	    collect_bay = CLAMP_SLOT_FRONT_BOTTOM;
	  }
	/* Destination. */
	if (!ctx.slots[build_bay])
	    ctx.moving_to = build_bay;
	else if (!ctx.slots[build_bay + 1])
	    ctx.moving_to = build_bay + 1;
	else
	  {
	    /* Build is finished. */
	    ctx.ready = 1;
	    return;
	  }
	/* Source. */
	if (ctx.slots[collect_bay + 2] &&
	    !ELEMENT_IS_HEAD (ctx.slots[collect_bay + 2]))
	    ctx.moving_from = collect_bay + 2;
	else if (ctx.slots[collect_bay] &&
		 !ELEMENT_IS_HEAD (ctx.slots[collect_bay]))
	    ctx.moving_from = collect_bay;
	else if (ctx.slots[CLAMP_SLOT_SIDE])
	    ctx.moving_from = CLAMP_SLOT_SIDE;
	return;
      }
    /* We have to prepare an element to put out. */
    else if (ctx.prepare == 2 && ctx.construct_possible)
      {
	logistic_put_element ();
      }
    /* Time to adjust element inside de robot, not build. */
    else
      {
	uint8_t get_bay, put_bay;
	if (ctx.collect_direction == DIRECTION_FORWARD)
	  {
	    get_bay = CLAMP_SLOT_FRONT_BOTTOM;
	    put_bay = CLAMP_SLOT_BACK_BOTTOM;
	  }
	else
	  {
	    get_bay = CLAMP_SLOT_BACK_BOTTOM;
	    put_bay = CLAMP_SLOT_FRONT_BOTTOM;
	  }
	/* Adjust some pawns. */
	/* Search source. */
	if (ctx.slots[get_bay] == ELEMENT_PAWN)
	    ctx.moving_from = get_bay;
	else if (ctx.slots[get_bay + 2] == ELEMENT_PAWN)
	    ctx.moving_from = get_bay + 2;
	else if (ctx.slots[put_bay + 2] == ELEMENT_PAWN)
	    ctx.moving_from = put_bay + 2;
	else if (ctx.slots[put_bay] == ELEMENT_PAWN)
	    ctx.moving_from = put_bay;

	/* Search destination. */
	if (ctx.moving_from != CLAMP_SLOT_NB)
	  {
	    if (!ctx.slots[CLAMP_SLOT_SIDE])
		ctx.moving_to = CLAMP_SLOT_SIDE;
	    else if (!ctx.slots[put_bay])
		ctx.moving_to = put_bay;
	    else if (!ctx.slots[put_bay + 2] && ctx.moving_from != put_bay)
		ctx.moving_to = put_bay + 2;
	    else
		ctx.moving_from = CLAMP_SLOT_NB;
	  }
	if (ctx.moving_from == ctx.moving_to)
	  {
	    ctx.moving_from = CLAMP_SLOT_NB;
	    ctx.moving_to = CLAMP_SLOT_NB;
	  }

	if (ctx.collect_direction == DIRECTION_FORWARD)
	    ctx.clamp_pos_idle = CLAMP_SLOT_FRONT_MIDDLE;
	else
	    ctx.clamp_pos_idle = CLAMP_SLOT_BACK_MIDDLE;
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
    ctx.construct_possible = 0;
    ctx.prepare = 0;
    ctx.ready = 0;
    ctx.need_prepare = 0;
}

void
logistic_update (void)
{
}

void
logistic_element_new (uint8_t pos, uint8_t element_type)
{
    assert (pos < CLAMP_SLOT_NB);
    assert (!ctx.slots[pos]);
    ctx.slots[pos] = element_type;
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

void
logistic_drop (uint8_t direction)
{
    uint8_t bay = direction == DIRECTION_FORWARD
	? CLAMP_SLOT_BACK_BOTTOM : CLAMP_SLOT_FRONT_BOTTOM;
    uint8_t i;
    for (i = bay; i < bay + 3; i++)
	ctx.slots[i] = 0;
    logistic_decision ();
}

static uint8_t
logistic_slot_clear (uint8_t slot)
{
    if (CLAMP_IS_SLOT_IN_FRONT_BAY (slot)
	&& ctx.slots[CLAMP_SLOT_FRONT_MIDDLE])
	return 0;
    if (CLAMP_IS_SLOT_IN_BACK_BAY (slot)
	&& ctx.slots[CLAMP_SLOT_BACK_MIDDLE])
	return 0;
    return 1;
}

uint8_t
logistic_path_clear (uint8_t slot1, uint8_t slot2)
{
    return logistic_slot_clear (slot1) && logistic_slot_clear (slot2);
}

