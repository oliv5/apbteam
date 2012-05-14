/* strat.c */
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
#include "common.h"
#include "strat.h"

#include "playground_2012.h"
#include "bot.h"
#include "path.h"

#include "asserv.h"

/*
 * This file implements strategic decisions.
 */

enum
{
    /** The four collecting places for totems. */
    STRAT_PLACE_TOTEM0,
    STRAT_PLACE_TOTEM1,
    STRAT_PLACE_TOTEM2,
    STRAT_PLACE_TOTEM3,
    /** Message in a bottle. */
    STRAT_PLACE_BOTTLE0,
    STRAT_PLACE_BOTTLE1,
    STRAT_PLACE_BOTTLE2,
    STRAT_PLACE_BOTTLE3,
    /** Number of places, should be last. */
    STRAT_PLACE_NB
};

/** Place static information. */
struct strat_place_t
{
    /** Collect position. */
    vect_t pos;
    /** Static score. */
    uint8_t score;
    /** Decision code. */
    uint8_t decision;
};
static const struct strat_place_t strat_place[STRAT_PLACE_NB] = {
      { { PG_WIDTH / 2 - PG_TOTEM_X_OFFSET_MM,
	  PG_LENGTH / 2 + PATH_PEANUT_CLEAR_MM }, 100, STRAT_DECISION_TOTEM },
      { { PG_WIDTH / 2 + PG_TOTEM_X_OFFSET_MM,
	  PG_LENGTH / 2 + PATH_PEANUT_CLEAR_MM }, 100, STRAT_DECISION_TOTEM },
      { { PG_WIDTH / 2 - PG_TOTEM_X_OFFSET_MM,
	  PG_LENGTH / 2 - PATH_PEANUT_CLEAR_MM }, 100, STRAT_DECISION_TOTEM },
      { { PG_WIDTH / 2 + PG_TOTEM_X_OFFSET_MM,
	  PG_LENGTH / 2 - PATH_PEANUT_CLEAR_MM }, 100, STRAT_DECISION_TOTEM },
      { { PG_BOTTLE0_X, BOT_SIZE_RADIUS + 70 }, 100, STRAT_DECISION_BOTTLE },
      { { PG_BOTTLE1_X, BOT_SIZE_RADIUS + 70 }, 100, STRAT_DECISION_BOTTLE },
      { { PG_BOTTLE2_X, BOT_SIZE_RADIUS + 70 }, 100, STRAT_DECISION_BOTTLE },
      { { PG_BOTTLE3_X, BOT_SIZE_RADIUS + 70 }, 100, STRAT_DECISION_BOTTLE },
};

/** Place dynamic information. */
struct strat_place_dyn_t
{
    /** Valid (not collected yet). */
    uint8_t valid;
    /** Tryed yet. */
    uint8_t fail_nb;
};

/** Strat context. */
struct strat_t
{
    /** Last decision. */
    uint8_t last_decision;
    /** Place of last decision. */
    uint8_t last_place;
    /** Robot content estimation. */
    uint8_t load;
    /** Places information. */
    struct strat_place_dyn_t place[STRAT_PLACE_NB];
};
static struct strat_t strat;

void
strat_init (void)
{
    uint8_t i;
    strat.last_decision = -1;
    for (i = 0; i < STRAT_PLACE_NB; i++)
	strat.place[i].valid = 1;
    if (team_color)
      {
	strat.place[STRAT_PLACE_BOTTLE1].valid = 0;
	strat.place[STRAT_PLACE_BOTTLE3].valid = 0;
      }
    else
      {
	strat.place[STRAT_PLACE_BOTTLE0].valid = 0;
	strat.place[STRAT_PLACE_BOTTLE2].valid = 0;
      }
}

/** Compute score for a path to the given position. */
static int32_t
strat_position_score (const vect_t *pos)
{
    uint16_t path_score;
    /* Find a path to position. */
    position_t current_pos;
    asserv_get_position (&current_pos);
    path_endpoints (current_pos.v, *pos);
    path_update ();
    path_score = path_get_score ();
    if (path_score != (uint16_t) -1)
	return path_score;
    else
      {
	path_escape (8);
	path_update ();
	path_score = path_get_score ();
	if (path_score != (uint16_t) -1)
	    return 4 * path_score;
	else
	    return -1;
      }
}

/** Compute score for a given place. */
static int32_t
strat_place_score (uint8_t i)
{
    if (!strat.place[i].valid)
	return -1;
    int32_t position_score = strat_position_score (&strat_place[i].pos);
    if (position_score == -1)
	return -1;
    return 10000 - position_score;
}

uint8_t
strat_decision (vect_t *pos)
{
    int32_t best_score = -1;
    uint8_t best_place = 0;
    uint8_t i;
    if (strat.load > 1)
      {
	strat.last_decision = STRAT_DECISION_UNLOAD;
	pos->x = PG_X (BOT_SIZE_RADIUS + 30);
	pos->y = PG_Y (PG_LENGTH / 2);
	return strat.last_decision;
      }
    for (i = 0; i < STRAT_PLACE_NB; i++)
      {
	int32_t score = strat_place_score (i);
	if (score > best_score)
	  {
	    best_score = score;
	    best_place = i;
	  }
      }
    if (best_score != -1)
      {
	*pos = strat_place[best_place].pos;
	strat.last_decision = strat_place[best_place].decision;
	strat.last_place = best_place;
	return strat.last_decision;
      }
    /* Nothing yet, crash. */
    return -1;
}

void
strat_success (void)
{
    switch (strat.last_decision)
      {
      case STRAT_DECISION_TOTEM:
	strat.load++;
	/* no break; */
      case STRAT_DECISION_BOTTLE:
	strat.place[strat.last_place].valid = 0;
	break;
      case STRAT_DECISION_UNLOAD:
	strat.load = 0;
	break;
      default:
	assert (0);
      }
}

void
strat_failure (void)
{
    switch (strat.last_decision)
      {
      default:
	if (strat.place[strat.last_place].fail_nb < 255)
	    strat.place[strat.last_place].fail_nb++;
	break;
      case STRAT_DECISION_UNLOAD:
	break;
      }
}

