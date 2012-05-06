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
	  PG_LENGTH / 2 + PATH_TOTEM_CLEAR_MM }, 100, STRAT_DECISION_TOTEM },
      { { PG_WIDTH / 2 + PG_TOTEM_X_OFFSET_MM,
	  PG_LENGTH / 2 + PATH_TOTEM_CLEAR_MM }, 100, STRAT_DECISION_TOTEM },
      { { PG_WIDTH / 2 - PG_TOTEM_X_OFFSET_MM,
	  PG_LENGTH / 2 - PATH_TOTEM_CLEAR_MM }, 100, STRAT_DECISION_TOTEM },
      { { PG_WIDTH / 2 + PG_TOTEM_X_OFFSET_MM,
	  PG_LENGTH / 2 - PATH_TOTEM_CLEAR_MM }, 100, STRAT_DECISION_TOTEM },
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

uint8_t
strat_decision (vect_t *pos)
{
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
	if (strat.place[i].valid)
	  {
	    *pos = strat_place[i].pos;
	    strat.last_decision = strat_place[i].decision;
	    strat.last_place = i;
	    return strat.last_decision;
	  }
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

