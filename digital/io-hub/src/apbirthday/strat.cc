// io-hub - Modular Input/Output. {{{
//
// Copyright (C) 2013 Nicolas Schodet
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
#include "strat.hh"
#include "robot.hh"
#include "top.hh"
#include "debug.host.hh"

Strat::Decision
Strat::decision (vect_t &pos)
{
    // TODO: this is a stub.
    pos = pg_cake_pos;
    return CANDLES;
}

/// Compute score for candles between first and last.
static int
strat_candles_score (int first, int last)
{
    int score = 0;
    Candles::Color other_color = team_color == TEAM_COLOR_RIGHT
        ? Candles::BLUE : Candles::RED;
    for (int i = first; i < last + 1; i++)
    {
        if (robot->candles.state[i] != Candles::PUNCHED
            && robot->candles.color[i] != other_color)
            score++;
    }
    return score;
}

bool
Strat::decision_candles (CandlesDecision &decision, uint16_t robot_angle)
{
    // Make an evaluation of the best direction to follow.
    // TODO: +1/-1 until candles at ends can be reached.
    int limit, score_forward, score_backward;
    limit = top_candle_for_angle (robot_angle, Candles::FAR, 1);
    score_backward = strat_candles_score (0 + 1, limit);
    score_forward = strat_candles_score (limit + 1, 7 - 1);
    limit = top_candle_for_angle (robot_angle, Candles::NEAR, 1);
    score_backward += strat_candles_score (8 + 1, limit);
    score_forward += strat_candles_score (limit + 1, 19 - 1);
    // Can not choose a direction with an obstacle.
    if (score_backward && top_follow_blocking (-1))
        score_backward = 0;
    if (score_forward && top_follow_blocking (1))
        score_forward = 0;
    // Now choose.
    host_debug ("score: forward = %d, backward = %d\n",
                score_forward, score_backward);
    if (score_forward == 0 && score_backward == 0)
    {
        return false;
    }
    else
    {
        // So near... let blow them...
        if (score_forward && robot_angle > G_ANGLE_UF016_DEG (-45))
            score_forward += 100;
        if (score_backward && robot_angle < G_ANGLE_UF016_DEG (180 + 45))
            score_backward += 100;
        // Compare.
        if (score_forward > score_backward)
        {
            decision.dir_sign = 1;
            decision.end_angle = G_ANGLE_UF016_DEG (-180. / 12);
        }
        else
        {
            decision.dir_sign = -1;
            decision.end_angle = G_ANGLE_UF016_DEG (180 + 180. / 12);
        }
        return true;
    }
}

void
Strat::failure ()
{
}

