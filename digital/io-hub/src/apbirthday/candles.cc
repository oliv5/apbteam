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
    // Init candles color.
    for (i = 0; i < total_count; i++)
    {
        color[i] = UNKNOWN;
        state[i] = UNPUNCHED;
    }
    color[0] = BLUE;
    color[8] = BLUE;
    color[7] = RED;
    color[19] = RED;
    if (calif_mode)
    {
        color[12] = WHITE;
        color[13] = WHITE;
        color[14] = WHITE;
        color[15] = WHITE;
    }
    actual_pos[NEAR] = -1;
    actual_pos[FAR] = -1;
}

void Candles::blow (int candle)
{
    if (is_far (candle))
        actual_pos[FAR] = candle;
    else
        actual_pos[NEAR] = candle;
    deduce ();
    robot->fsm_queue.post (FSM_EVENT (ai_candle_blow));
}

void Candles::deduce ()
{
    int i;
    // Far.
    for (i = 1; i < 4; i++)
        if ((color[i] == UNKNOWN) ^ (color[7 - i] == UNKNOWN))
        {
            if (color[i] == RED)
                color[7 - i] = BLUE;
            else if (color[i] == BLUE)
                color[7 - i] = RED;
            else if (color[7 - i] == RED)
                color[i] = BLUE;
            else if (color[7 - i] == BLUE)
                color[i] = RED;
        }
    // Near.
    for (i = 1; i < 6; i++)
        if ((color[8 + i] == UNKNOWN) ^ (color[19 - i] == UNKNOWN))
        {
            if (color[8 + i] == RED)
                color[19 - i] = BLUE;
            else if (color[8 + i] == BLUE)
                color[19 - i] = RED;
            else if (color[19 - i] == RED)
                color[8 + i] = BLUE;
            else if (color[19 - i] == BLUE)
                color[8 + i] = RED;
        }
}

inline bool Candles::is_near (int pos)
{
    return pos >= far_count;
}


inline bool Candles::is_far (int pos)
{
    return pos < far_count;
}

void Candles::deploy_arm ()
{
    // Deploy arm.
    robot->hardware.cake_arm_out.set (true);
    robot->hardware.cake_arm_in.set (false);
    // Prepare punhers.
    unpush_far ();
    unpush_near ();
}

void Candles::undeploy_arm_1 ()
{
    // Get a flabby arm (usefull in case of failure).
     robot->hardware.cake_arm_out.set (false);
     robot->hardware.cake_arm_in.set (false);
    // Prepare far puncher to undeploy.
    Candles::push_far ();
    // Be sure the near punched in not punching.
    Candles::unpush_near ();
}

void Candles::undeploy_arm_2 ()
{
    // Unleach far puncher.
    robot->hardware.cake_push_far_out.set (false);
    robot->hardware.cake_push_far_in.set (false);
    // Put arm back.
    robot->hardware.cake_arm_out.set (false);
    robot->hardware.cake_arm_in.set (true);
}

void Candles::push_near ()
{
    robot->hardware.cake_push_near_out.set (true);
    robot->hardware.cake_push_near_in.set (false);
}

void Candles::unpush_near ()
{
    robot->hardware.cake_push_near_out.set (false);
    robot->hardware.cake_push_near_in.set (true);
}

void Candles::push_far ()
{
    robot->hardware.cake_push_far_out.set (true);
    robot->hardware.cake_push_far_in.set (false);
}

void Candles::unpush_far ()
{
    robot->hardware.cake_push_far_out.set (false);
    robot->hardware.cake_push_far_in.set (true);
}

// Global candle FSM.
FSM_STATES (AI_CANDLE_OFF,
            AI_CANDLE_INIT,
            AI_CANDLE_SLEEPING,
            AI_CANDLE_DEPLOYING,
            AI_CANDLE_FALLING_BACK_TO_UNDEPLOYED,
            AI_CANDLE_READY,
            AI_CANDLE_UNDEPLOYING,
            AI_CANDLE_UNDEPLOYING_2)

FSM_EVENTS (ai_candle_deploy,
            ai_candle_undeploy,
            ai_candle_success,
            ai_candle_failure,
            ai_candle_blow)

FSM_START_WITH (AI_CANDLE_OFF)

FSM_TRANS (AI_CANDLE_OFF, init_actuators, AI_CANDLE_INIT)
{
    Candles::deploy_arm ();
}

FSM_TRANS_TIMEOUT (AI_CANDLE_INIT, 12, AI_CANDLE_UNDEPLOYING)
{
    Candles::undeploy_arm_1 ();
}

FSM_TRANS (AI_CANDLE_SLEEPING, ai_candle_deploy, AI_CANDLE_DEPLOYING)
{
    Candles::deploy_arm ();
}

FSM_TRANS (AI_CANDLE_SLEEPING, ai_candle_undeploy, AI_CANDLE_SLEEPING)
{
    robot->fsm_queue.post (FSM_EVENT (ai_candle_success));
}

FSM_TRANS_TIMEOUT (AI_CANDLE_DEPLOYING, 125,
                   success, AI_CANDLE_READY,
                   failure, AI_CANDLE_FALLING_BACK_TO_UNDEPLOYED)
{
    if (!robot->hardware.cake_arm_out_contact.get ())
    {
        robot->fsm_queue.post (FSM_EVENT (ai_candle_success));
        return FSM_BRANCH (success);
    }
    else
    {
        // Get back to Sleep.
        Candles::undeploy_arm_1 ();
        robot->fsm_queue.post (FSM_EVENT (ai_candle_failure));
        return FSM_BRANCH (failure);
    }
}

FSM_TRANS_TIMEOUT (AI_CANDLE_FALLING_BACK_TO_UNDEPLOYED, 12, AI_CANDLE_SLEEPING)
{
    Candles::undeploy_arm_2 ();
}

FSM_TRANS (AI_CANDLE_READY, ai_candle_blow, AI_CANDLE_READY)
{
    int i;
    for (i = 0; i < Candles::FLOOR_NB; i++)
    {
        if (robot->candles.actual_pos[i] != -1)
        {
            // We can already punch if we know the color.
            if (robot->candles.state[robot->candles.actual_pos[i]] == Candles::UNPUNCHED
                    && (robot->candles.color[robot->candles.actual_pos[i]] == (Candles::Color) team_color
                        || robot->candles.color[robot->candles.actual_pos[i]] == Candles::WHITE))
            {
                if (Candles::is_far (robot->candles.actual_pos[i]))
                    FSM_HANDLE (AI, ai_candle_far_punch);
                else
                    FSM_HANDLE (AI, ai_candle_near_punch);
                robot->candles.state[robot->candles.actual_pos[i]] = Candles::PUNCHED;
                robot->candles.actual_pos[i] = -1;
            }
            // We need to analyse color.
            else if (robot->candles.color[robot->candles.actual_pos[i]] == Candles::UNKNOWN)
            {
                if (Candles::is_far (robot->candles.actual_pos[i]))
                    FSM_HANDLE (AI, ai_candle_far_analyse);
                else
                    FSM_HANDLE (AI, ai_candle_near_analyse);
            }
        }
    }
}

FSM_TRANS (AI_CANDLE_READY, ai_candle_deploy, AI_CANDLE_READY)
{
    robot->fsm_queue.post (FSM_EVENT (ai_candle_success));
}

FSM_TRANS (AI_CANDLE_READY, ai_candle_undeploy, AI_CANDLE_UNDEPLOYING)
{
    Candles::undeploy_arm_1 ();
}

FSM_TRANS_TIMEOUT (AI_CANDLE_UNDEPLOYING, 10, AI_CANDLE_UNDEPLOYING_2) //TODO timeout value
{
    Candles::undeploy_arm_2 ();
}

FSM_TRANS_TIMEOUT (AI_CANDLE_UNDEPLOYING_2, 125,
                   success, AI_CANDLE_SLEEPING,
                   failure, AI_CANDLE_READY)
{
    if (!robot->hardware.cake_arm_in_contact.get ())
    {
        robot->fsm_queue.post (FSM_EVENT (ai_candle_success));
        return FSM_BRANCH (success);
    }
    else
    {
        Candles::deploy_arm ();
        robot->fsm_queue.post (FSM_EVENT (ai_candle_failure));
        return FSM_BRANCH (failure);
    }
}

// Far puncher FSM.
FSM_STATES (AI_CANDLE_FAR_SLEEPING,
            AI_CANDLE_FAR_PUNCHING)

FSM_EVENTS (ai_candle_far_punch)

FSM_START_WITH (AI_CANDLE_FAR_SLEEPING)

FSM_TRANS (AI_CANDLE_FAR_SLEEPING, ai_candle_far_punch, AI_CANDLE_FAR_PUNCHING)
{
    Candles::push_far ();
}

FSM_TRANS_TIMEOUT (AI_CANDLE_FAR_PUNCHING, 25, AI_CANDLE_FAR_SLEEPING) //TODO timeout value
{
    Candles::unpush_far ();
}

// Near puncher FSM.
FSM_STATES (AI_CANDLE_NEAR_SLEEPING,
            AI_CANDLE_NEAR_PUNCHING)

FSM_EVENTS (ai_candle_near_punch)

FSM_START_WITH (AI_CANDLE_NEAR_SLEEPING)

FSM_TRANS (AI_CANDLE_NEAR_SLEEPING, ai_candle_near_punch, AI_CANDLE_NEAR_PUNCHING)
{
    Candles::push_near ();
}

FSM_TRANS_TIMEOUT (AI_CANDLE_NEAR_PUNCHING, 25, AI_CANDLE_NEAR_SLEEPING) //TODO timeout value
{
    Candles::unpush_near ();
}

// Far analyse FSM.
FSM_STATES (AI_CANDLE_FAR_ANALYSE_SLEEP,
            AI_CANDLE_FAR_ANALYSING)

FSM_EVENTS (ai_candle_far_analyse)

FSM_START_WITH (AI_CANDLE_FAR_ANALYSE_SLEEP)

FSM_TRANS (AI_CANDLE_FAR_ANALYSE_SLEEP,
           ai_candle_far_analyse,
           AI_CANDLE_FAR_ANALYSING)
{
    // TODO: launch color analyse.
}

FSM_TRANS_TIMEOUT (AI_CANDLE_FAR_ANALYSING, 10, AI_CANDLE_FAR_ANALYSE_SLEEP) //TODO timeout value
{
    // TODO Get color results and update table.
    // ...
    // Update color.
    if (true) // TODO color analysise is ok
    {
        robot->candles.color[robot->candles.actual_pos[Candles::FAR]] =
            Candles::RED; // TODO = color_result
        // Update whole colors.
        robot->candles.deduce ();
        // Send blow event.
        FSM_HANDLE (AI, ai_candle_blow);
    }
    else
    {
        // Too bad, we do not have a valid color analysis.
        robot->candles.actual_pos[Candles::FAR] = -1;
    }
}

// Near analyse FSM.
FSM_STATES (AI_CANDLE_NEAR_ANALYSE_SLEEP,
            AI_CANDLE_NEAR_ANALYSING)

FSM_EVENTS (ai_candle_near_analyse)

FSM_START_WITH (AI_CANDLE_NEAR_ANALYSE_SLEEP)

FSM_TRANS (AI_CANDLE_NEAR_ANALYSE_SLEEP,
           ai_candle_near_analyse,
           AI_CANDLE_NEAR_ANALYSING)
{
    // TODO: Launch color analyse.
}

FSM_TRANS_TIMEOUT (AI_CANDLE_NEAR_ANALYSING, 10, AI_CANDLE_NEAR_ANALYSE_SLEEP) //TODO timeout value
{
    // TODO Get color results and update table.
    // ...
    // Update color.
    if (true) // TODO color analysise is ok
    {
        robot->candles.color[robot->candles.actual_pos[Candles::NEAR]] =
            Candles::RED; // TODO = color_result
        // Update whole colors.
        robot->candles.deduce ();
        // Send blow event.
        FSM_HANDLE (AI, ai_candle_blow);
    }
    else
    {
        // Too bad, we do not have a valid color analysis.
        robot->candles.actual_pos[Candles::NEAR] = -1;
    }
}
