// io-hub - Modular Input/Output. {{{
//
// Copyright (C) 2013 Jerome Jutteau
//
// APBTeam:
//        Web: http://apbteam.org/
//      Email: team AT apbteam DOT org
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

#include "robot.hh"
#include "defs.hh"
#include "candles.hh"

extern "C" {
#define ANGFSM_NAME AI
#include "angfsm.h"
}

Candles::Candles (int calif_mode)
{
    int i;
    /* Init candles color. */
    for (i = 0; i < CANDLES_NB; i++)
    {
        color[i] = CANDLE_UNKNOWN;
        state[i] = CANDLE_UNPUNCHED;
    }
    color[0] = CANDLE_BLUE;
    color[8] = CANDLE_BLUE;
    color[7] = CANDLE_RED;
    color[19] = CANDLE_RED;
    if (calif_mode)
    {
        color[12] = CANDLE_WHITE;
        color[13] = CANDLE_WHITE;
        color[14] = CANDLE_WHITE;
        color[15] = CANDLE_WHITE;
    }
    actual_pos[FLOOR_NEAR] = CANDLES_NB;
    actual_pos[FLOOR_FAR] = CANDLES_NB;
}

void Candles::blow (int candle)
{
    if (CANDLE_IS_FAR(candle))
        actual_pos[FLOOR_FAR] = candle;
    else
        actual_pos[FLOOR_NEAR] = candle;
    deduce ();
    robot->fsm_queue.post (FSM_EVENT (ai_candle_blow));
}

void Candles::deduce ()
{
    int i;
    /* Far. */
    for (i = 1; i < 4; i++)
        if (color[i] != color[i + 3])
        {
            if (color[i] == CANDLE_RED)
                color[i + 3] = CANDLE_BLUE;
            else if (color[i] == CANDLE_BLUE)
                color[i + 3] = CANDLE_RED;
            else if (color[i + 3] == CANDLE_RED)
                color[i] = CANDLE_BLUE;
            else if (color[i + 3] == CANDLE_BLUE)
                color[i] = CANDLE_RED;
        }
    /* Near. */
    for (i = 9; i < 14; i++)
        if (color[i] != color[i + 5])
        {
            if (color[i] == CANDLE_RED)
                color[i + 5] = CANDLE_BLUE;
            else if (color[i] == CANDLE_BLUE)
                color[i + 5] = CANDLE_RED;
            else if (color[i + 5] == CANDLE_RED)
                color[i] = CANDLE_BLUE;
            else if (color[i + 5] == CANDLE_BLUE)
                color[i] = CANDLE_RED;
        }
}

/* Global candle FSM */
FSM_STATES (AI_CANDLE_SLEEPING,
        AI_CANDLE_READY,
        AI_CANDLE_UNDEPLOYING)

FSM_EVENTS (ai_candle_deploy,
        ai_candle_undeploy,
        ai_candle_blow)

FSM_START_WITH (AI_CANDLE_SLEEPING)

FSM_TRANS (AI_CANDLE_SLEEPING, ai_candle_deploy, AI_CANDLE_READY)
{
    /* TODO: piston arm IN */
    /* TODO: piston far OUT */
}

FSM_TRANS (AI_CANDLE_READY, ai_candle_blow, AI_CANDLE_READY)
{
    int i;
    for (i = 0; i < CANDLE_FLOOR_NB; i++)
    {
        if (robot->candles.actual_pos[i] != CANDLE_FLOOR_NB)
        {
            /* We can already punch if we know the color. */
            if (robot->candles.state[robot->candles.actual_pos[i]] == CANDLE_UNPUNCHED
                    && (robot->candles.color[robot->candles.actual_pos[i]] == (candle_color_t) team_color
                        || robot->candles.color[robot->candles.actual_pos[i]] == CANDLE_WHITE))
            {
                if (CANDLE_IS_FAR (robot->candles.actual_pos[i]))
                    FSM_HANDLE (AI, ai_candle_far_punch);
                else
                    FSM_HANDLE (AI, ai_candle_near_punch);
                robot->candles.state[robot->candles.actual_pos[i]] = CANDLE_PUNCHED;
                robot->candles.actual_pos[i] = CANDLE_FLOOR_NB;
            }
            /* We need to analyse color. */
            else if (robot->candles.color[robot->candles.actual_pos[i]] == CANDLE_UNKNOWN)
            {
                if (CANDLE_IS_FAR (robot->candles.actual_pos[i]))
                    FSM_HANDLE (AI, ai_candle_far_analyse);
                else
                    FSM_HANDLE (AI, ai_candle_near_analyse);
            }
        }
    }
}

FSM_TRANS (AI_CANDLE_READY, ai_candle_undeploy, AI_CANDLE_UNDEPLOYING)
{
    /* TODO: piston far OUT */
}

FSM_TRANS_TIMEOUT (AI_CANDLE_UNDEPLOYING, 10, AI_CANDLE_SLEEPING) //TODO timeout value
{
    /* TODO: piston far unleach */
    /* TODO: piston arm IN */
}

/* Far puncher FSM */
FSM_STATES (AI_CANDLE_FAR_SLEEPING,
        AI_CANDLE_FAR_PUNCHING)

FSM_EVENTS (ai_candle_far_punch)

FSM_START_WITH (AI_CANDLE_FAR_SLEEPING)

FSM_TRANS (AI_CANDLE_FAR_SLEEPING, ai_candle_far_punch, AI_CANDLE_FAR_PUNCHING)
{
    /* TODO: piston far OUT */
}

FSM_TRANS_TIMEOUT (AI_CANDLE_FAR_PUNCHING, 12, AI_CANDLE_FAR_SLEEPING) //TODO timeout value
{
    /* TODO: piston far OUT */
}

/* Near puncher FSM */
FSM_STATES (AI_CANDLE_NEAR_SLEEPING,
        AI_CANDLE_NEAR_PUNCHING)

FSM_EVENTS (ai_candle_near_punch)

FSM_START_WITH (AI_CANDLE_NEAR_SLEEPING)

FSM_TRANS (AI_CANDLE_NEAR_SLEEPING, ai_candle_near_punch, AI_CANDLE_NEAR_PUNCHING)
{
    /* TODO: piston near OUT */
}

FSM_TRANS_TIMEOUT (AI_CANDLE_NEAR_PUNCHING, 12, AI_CANDLE_NEAR_SLEEPING) //TODO timeout value
{
    /* TODO: piston near OUT */
}

/* Far analyse FSM */
FSM_STATES (AI_CANDLE_FAR_ANALYSE_SLEEP,
        AI_CANDLE_FAR_ANALYSING)

FSM_EVENTS (ai_candle_far_analyse)

FSM_START_WITH (AI_CANDLE_FAR_ANALYSE_SLEEP)

FSM_TRANS (AI_CANDLE_FAR_ANALYSE_SLEEP,
        ai_candle_far_analyse,
        AI_CANDLE_FAR_ANALYSING)
{
    /* TODO: launch color analyse */
}

FSM_TRANS_TIMEOUT (AI_CANDLE_FAR_ANALYSING, 10, AI_CANDLE_FAR_ANALYSE_SLEEP) //TODO timeout value
{
    /* TODO Get color results and update table. */
    /* ... */
    /* Update color. */
    robot->candles.color[robot->candles.actual_pos[FLOOR_FAR]] = CANDLE_RED; // TODO = color_result
    /* Update whole colors. */
    robot->candles.deduce ();
    /* Send blow event. */
    FSM_HANDLE (AI, ai_candle_blow);
}

/* Near analyse FSM */
FSM_STATES (AI_CANDLE_NEAR_ANALYSE_SLEEP,
        AI_CANDLE_NEAR_ANALYSING)

FSM_EVENTS (ai_candle_near_analyse)

FSM_START_WITH (AI_CANDLE_NEAR_ANALYSE_SLEEP)

FSM_TRANS (AI_CANDLE_NEAR_ANALYSE_SLEEP,
        ai_candle_near_analyse,
        AI_CANDLE_NEAR_ANALYSING)
{
    /* TODO: Launch color analyse. */
}

FSM_TRANS_TIMEOUT (AI_CANDLE_NEAR_ANALYSING, 10, AI_CANDLE_NEAR_ANALYSE_SLEEP) //TODO timeout value
{
    /* TODO Get color results and update table. */
    /* ... */
    /* Update color. */
    robot->candles.color[robot->candles.actual_pos[FLOOR_NEAR]] = CANDLE_RED; // TODO
    /* Update whole colors. */
    robot->candles.deduce ();
    /* Send blow event. */
    FSM_HANDLE (AI, ai_candle_blow);
}
