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
#include "top.hh"
#include "robot.hh"
#include "playground_2013.hh"
#include "bot.hh"

#include <cmath>

/// Top context.
struct top_t
{
    /// Candles decision information.
    Strat::CandlesDecision candles;
    /// Last blown candles.
    int candles_last_blown[Candles::FLOOR_NB];
    /// Plate decision information.
    Strat::PlateDecision plate;
};
static top_t top;

/// Compute the angle of the normal to the cake at the given point.
static uint16_t
top_cake_angle (const vect_t &pos)
{
    float dx = pos.x - pg_cake_pos.x;
    float dy = pos.y - pg_cake_pos.y;
    float angle_rad = std::atan2 (dy, dx);
    uint16_t angle = angle_rad * ((1 << 16) / (2 * M_PI));
    return angle;
}

/// Compute the angle of the normal to the cake at the robot position.
static uint16_t
top_cake_angle_robot ()
{
    Position pos = robot->asserv.get_position ();
    return top_cake_angle (pos.v);
}

int
top_candle_for_angle (uint16_t a, Candles::Floor floor, int dir_sign)
{
    /// Information for each floor.
    struct FloorInfo
    {
        /// Angle of the first candle, also include angle offset due
        /// to arm position.
        uint16_t first_angle;
        /// Index of the first candle.
        int first_index;
        /// Angle between candles.
        uint16_t inter_angle;
    };
    static const FloorInfo floor_info[Candles::FLOOR_NB] =
    {
#define NEAR_ARM_OFFSET_DEG 0
#define FAR_ARM_OFFSET_DEG 9
        { G_ANGLE_UF016_DEG (180 + 180. / (12 * 2) - NEAR_ARM_OFFSET_DEG), 8,
            G_ANGLE_UF016_DEG (180. / 12) },
        { G_ANGLE_UF016_DEG (180 + 180. / (8 * 2) - FAR_ARM_OFFSET_DEG), 0,
            G_ANGLE_UF016_DEG (180. / 8) },
    };
    // Compute for the forward direction.
    const FloorInfo &i = floor_info[floor];
    int index = i.first_index + (a - i.first_angle) / i.inter_angle;
    // For backward, add 1.
    if (dir_sign == -1)
        index++;
    return index;
}

/// Start follow mode, ask Strat for what to do.
static bool
top_follow_start ()
{
    uint16_t robot_angle = top_cake_angle_robot ();
    bool go_candle = robot->strat.decision_candles (top.candles, robot_angle);
    if (go_candle)
    {
        for (int floor = Candles::NEAR; floor < Candles::FLOOR_NB; floor++)
        {
            top.candles_last_blown[floor] =
                top_candle_for_angle (robot_angle, Candles::Floor (floor),
                                      top.candles.dir_sign);
        }
    }
    return go_candle;
}

/// Can follow cake, decide if this is a good thing, or leave.
static Branch
top_follow_or_leave ()
{
    if (top_follow_start ())
    {
        robot->asserv.follow (top.candles.dir_sign == 1
                              ? Asserv::FORWARD : Asserv::BACKWARD);
        return FSM_BRANCH (candles);
    }
    else
    {
        Position robot_pos = robot->asserv.get_position ();
        int dist = 0;
        // If near a border, need to move before turning.
        if (robot_pos.v.y > pg_length - BOT_SIZE_RADIUS - 30)
        {
            if (robot_pos.a < G_ANGLE_UF016_DEG (180))
                dist = - (BOT_SIZE_RADIUS + 30 - BOT_SIZE_FRONT);
            else
                dist = BOT_SIZE_RADIUS + 30 - BOT_SIZE_BACK;
        }
        // If near an obstacle, also need to move to undeploy arm.
        else if (top_follow_blocking (1))
        {
            dist = -100;
        }
        // Do not use move_distance, it depends too much on current robot
        // orientation which is not stable.
        uint16_t robot_angle = top_cake_angle (robot_pos.v);
        if (dist)
        {
            vect_t dst;
            vect_from_polar_uf016 (&dst, dist,
                                   robot_angle + G_ANGLE_UF016_DEG (90));
            vect_translate (&dst, &robot_pos.v);
            robot->asserv.goto_xy (dst, Asserv::REVERT_OK);
            return FSM_BRANCH (tangent);
        }
        else
        {
            robot->asserv.goto_angle (robot_angle);
            return FSM_BRANCH (turn);
        }
    }
}

bool
top_follow_blocking (int dir_sign)
{
    Position robot_pos = robot->asserv.get_position ();
    uint16_t robot_angle = top_cake_angle (robot_pos.v);
    // Check for an obstacle on a small segment.
    vect_t dst;
    uint16_t dst_angle = robot_angle + dir_sign * G_ANGLE_UF016_DEG (30);
    vect_from_polar_uf016 (&dst, pg_cake_radius + pg_cake_distance
                           + BOT_SIZE_SIDE, dst_angle);
    vect_translate (&dst, &pg_cake_pos);
    return robot->obstacles.blocking (robot_pos.v, dst, 200);
}

void
top_update ()
{
    if (FSM_CAN_HANDLE (AI, top_follow_finished))
    {
        // Update consign.
        int cons;
        const int k = 200;
#ifdef TARGET_host
        const int front_offset = 0x07fb;
        const int back_offset = 0x09af;
#else
        const int front_offset = 0x0762;
        const int back_offset = 0x08b3;
#endif
        if (top.candles.dir_sign == 1)
            cons = - robot->hardware.adc_cake_front.read () + front_offset;
        else
            cons = robot->hardware.adc_cake_back.read () - back_offset;
        robot->asserv.follow_update (cons * k / 1000);
    }
}

bool
top_fsm_gen_event ()
{
    if (ANGFSM_CAN_HANDLE (AI, top_follow_finished))
    {
        uint16_t robot_angle = top_cake_angle_robot ();
        int dir_sign = top.candles.dir_sign;
        // Check for movement end.
        if ((robot_angle - top.candles.end_angle) * dir_sign > 0)
        {
            if (ANGFSM_HANDLE (AI, top_follow_finished))
                return true;
        }
        // Check for obstacle.
        if (top_follow_blocking (dir_sign))
            if (ANGFSM_HANDLE (AI, top_follow_blocked))
                return true;
        // Check for a candle to blow.
        for (int floor = Candles::NEAR; floor < Candles::FLOOR_NB; floor++)
        {
            int candle = top_candle_for_angle (robot_angle,
                                               Candles::Floor (floor),
                                               top.candles.dir_sign);
            if (candle != top.candles_last_blown[floor])
            {
                robot->candles.blow (candle);
                top.candles_last_blown[floor] = candle;
            }
        }
    }
    // Plate contacts.
    if (!robot->hardware.cherry_plate_left_contact.get ()
        && !robot->hardware.cherry_plate_right_contact.get ())
    {
        if (ANGFSM_HANDLE (AI, top_plate_present))
            return true;
    }
    return false;
}

void
top_demo_follow (int dir_sign)
{
    top.candles.dir_sign = dir_sign;
    ANGFSM_HANDLE (AI, top_demo_follow);
}

ANGFSM_INIT

ANGFSM_STATES (
            // Initial state.
            TOP_START,
            // Initialising actuators.
            TOP_INIT_ACTUATORS,
            // Init done, waiting for rount start.
            TOP_INIT,
            // Decision state, one stop, one cycle.
            TOP_DECISION,
            // Candles: go to cake, with a normal move.
            TOP_CANDLES_GOTO_NORMAL,
            // Candles: deploy arm.
            TOP_CANDLES_ENTER_DEPLOY,
            // Candles: turn to be in position to follow the cake.
            TOP_CANDLES_ENTER_TURN,
            // Candles: follow the cake curve until destination angle, blowing
            // candles on the way.
            TOP_CANDLES_FOLLOW,
            // Candles: tangent move to escape from an obstacle.
            TOP_CANDLES_LEAVE_TANGENT_MOVE,
            // Candles: turn to leave, undeploy arm as soon as possible.
            TOP_CANDLES_LEAVE_TURN,
            // Candles: go away so that the robot is free to turn.
            TOP_CANDLES_LEAVE_GO_AWAY,
            // Plate: go to plate, normal move.
            TOP_PLATE_GOTO,
            // Plate: go backward until a plate is seen.
            TOP_PLATE_APPROACH,
            // Plate: loading plate.
            TOP_PLATE_LOADING,
            // Plate: drop plate.
            TOP_PLATE_DROPING,
            // Demo mode: push the wall near the cake.
            TOP_DEMO_CANDLES_PUSH_WALL,
            // Demo mode: move away from the wall.
            TOP_DEMO_CANDLES_MOVE_AWAY,
            // Demo mode: follow the cake (or anything else actually).
            TOP_DEMO_FOLLOW)

ANGFSM_EVENTS (
            // Cake following finished (end point reached).
            top_follow_finished,
            // Problem with cake following.
            top_follow_blocked,
            // Plate present, can be taken.
            top_plate_present,
            // Start candle demo.
            top_demo_candles,
            // Start follow the cake demo.
            top_demo_follow)

ANGFSM_START_WITH (TOP_START)

FSM_TRANS (TOP_START, init_actuators, TOP_INIT_ACTUATORS)
{
    // TODO: make sure the operator do not forget this is demo mode!
    robot->demo = !robot->hardware.ihm_strat.get ();
}

FSM_TRANS (TOP_INIT_ACTUATORS, init_done, TOP_INIT)
{
    // Color dependent init can go here.
}

FSM_TRANS (TOP_INIT, init_start_round, TOP_DECISION)
{
}

FSM_TRANS_TIMEOUT (TOP_DECISION, 1,
                   candles, TOP_CANDLES_GOTO_NORMAL,
                   plate, TOP_PLATE_GOTO,
                   none, TOP_START)
{
    if (robot->demo)
        return FSM_BRANCH (none);
    robot->asserv.set_speed (BOT_SPEED_NORMAL);
    Position d_pos;
    Strat::Decision d = robot->strat.decision (d_pos);
    switch (d)
    {
    case Strat::CANDLES:
        robot->move.start (d_pos.v, Asserv::BACKWARD, pg_cake_radius
                           + pg_cake_distance + BOT_SIZE_SIDE);
        return FSM_BRANCH (candles);
    case Strat::PLATE:
        robot->strat.decision_plate (top.plate);
        robot->move.start (d_pos);
        return FSM_BRANCH (plate);
    default:
        ucoo::assert_unreachable ();
    }
}

///
/// Candles.
///

FSM_TRANS (TOP_CANDLES_GOTO_NORMAL, move_success, TOP_CANDLES_ENTER_DEPLOY)
{
    ANGFSM_HANDLE (AI, ai_candle_deploy);
}

FSM_TRANS (TOP_CANDLES_GOTO_NORMAL, move_failure, TOP_DECISION)
{
    robot->strat.failure ();
}

FSM_TRANS (TOP_CANDLES_ENTER_DEPLOY, ai_candle_success, TOP_CANDLES_ENTER_TURN)
{
    robot->asserv.goto_angle (top_cake_angle_robot ()
                              + G_ANGLE_UF016_DEG (90));
}

FSM_TRANS (TOP_CANDLES_ENTER_DEPLOY, ai_candle_failure,
           TOP_CANDLES_LEAVE_GO_AWAY)
{
    robot->asserv.move_distance (BOT_SIZE_RADIUS - BOT_SIZE_SIDE);
}

FSM_TRANS (TOP_CANDLES_ENTER_TURN, robot_move_success,
           tangent, TOP_CANDLES_LEAVE_TANGENT_MOVE,
           turn, TOP_CANDLES_LEAVE_TURN,
           candles, TOP_CANDLES_FOLLOW)
{
    return top_follow_or_leave ();
}

FSM_TRANS (TOP_CANDLES_ENTER_TURN, robot_move_failure, TOP_CANDLES_LEAVE_TURN)
{
    robot->asserv.goto_angle (top_cake_angle_robot ());
}

FSM_TRANS (TOP_CANDLES_FOLLOW, top_follow_finished,
           tangent, TOP_CANDLES_LEAVE_TANGENT_MOVE,
           turn, TOP_CANDLES_LEAVE_TURN,
           candles, TOP_CANDLES_FOLLOW)
{
    return top_follow_or_leave ();
}

FSM_TRANS (TOP_CANDLES_FOLLOW, top_follow_blocked,
           tangent, TOP_CANDLES_LEAVE_TANGENT_MOVE,
           turn, TOP_CANDLES_LEAVE_TURN,
           candles, TOP_CANDLES_FOLLOW)
{
    return top_follow_or_leave ();
}

FSM_TRANS (TOP_CANDLES_FOLLOW, robot_move_failure,
           tangent, TOP_CANDLES_LEAVE_TANGENT_MOVE,
           turn, TOP_CANDLES_LEAVE_TURN,
           candles, TOP_CANDLES_FOLLOW)
{
    return top_follow_or_leave ();
}

FSM_TRANS (TOP_CANDLES_LEAVE_TANGENT_MOVE, robot_move_success,
           TOP_CANDLES_LEAVE_TURN)
{
    robot->asserv.goto_angle (top_cake_angle_robot ());
}

FSM_TRANS (TOP_CANDLES_LEAVE_TANGENT_MOVE, robot_move_failure,
           TOP_CANDLES_LEAVE_TURN)
{
    robot->asserv.goto_angle (top_cake_angle_robot ());
}

FSM_TRANS (TOP_CANDLES_LEAVE_TURN, robot_move_success,
           TOP_CANDLES_LEAVE_GO_AWAY)
{
    // TODO: undeploy earlier, by computing arm end position.
    ANGFSM_HANDLE (AI, ai_candle_undeploy);
    robot->asserv.move_distance (BOT_SIZE_RADIUS - BOT_SIZE_SIDE);
}

FSM_TRANS (TOP_CANDLES_LEAVE_TURN, robot_move_failure,
           TOP_CANDLES_LEAVE_GO_AWAY)
{
    ANGFSM_HANDLE (AI, ai_candle_undeploy);
    robot->asserv.move_distance (BOT_SIZE_RADIUS - BOT_SIZE_SIDE);
}

FSM_TRANS (TOP_CANDLES_LEAVE_GO_AWAY, robot_move_success, TOP_DECISION)
{
}

FSM_TRANS (TOP_CANDLES_LEAVE_GO_AWAY, robot_move_failure, TOP_DECISION)
{
}

///
/// Plate.
///

FSM_TRANS (TOP_PLATE_GOTO, move_success,
           load, TOP_PLATE_APPROACH,
           drop, TOP_PLATE_DROPING)
{
    if (top.plate.drop)
    {
        ANGFSM_HANDLE (AI, plate_drop);
        return FSM_BRANCH (drop);
    }
    else
    {
        robot->asserv.set_speed (BOT_SPEED_PLATE);
        robot->move.start (top.plate.loading_pos, Asserv::BACKWARD);
        return FSM_BRANCH (load);
    }
}

FSM_TRANS (TOP_PLATE_GOTO, move_failure, TOP_DECISION)
{
    robot->strat.failure ();
}

FSM_TRANS (TOP_PLATE_DROPING, plate_droped, TOP_PLATE_GOTO)
{
    top.plate.drop = false;
    robot->move.start (top.plate.approaching_pos);
}

FSM_TRANS (TOP_PLATE_APPROACH, move_success, TOP_DECISION)
{
    // TODO: no plate.
    robot->strat.failure ();
}

FSM_TRANS (TOP_PLATE_APPROACH, move_failure, TOP_DECISION)
{
    robot->strat.failure ();
}

FSM_TRANS (TOP_PLATE_APPROACH, top_plate_present, TOP_PLATE_LOADING)
{
    robot->move.stop ();
    ANGFSM_HANDLE (AI, plate_take);
}

FSM_TRANS (TOP_PLATE_LOADING, plate_taken, TOP_DECISION)
{
}

///
/// Demo mode.
///

FSM_TRANS (TOP_INIT_ACTUATORS, top_demo_candles, TOP_DEMO_CANDLES_PUSH_WALL)
{
    team_color = TEAM_COLOR_LEFT;
    robot->asserv.push_wall (Asserv::FORWARD, pg_cake_pos.x - pg_cake_radius
                             - 20 - BOT_SIZE_SIDE,
                             pg_length - BOT_FRONT_CONTACT_DIST,
                             G_ANGLE_UF016_DEG (90));
}

FSM_TRANS (TOP_DEMO_CANDLES_PUSH_WALL, robot_move_success,
           TOP_DEMO_CANDLES_MOVE_AWAY)
{
    robot->asserv.move_distance (-200);
}

FSM_TRANS (TOP_DEMO_CANDLES_MOVE_AWAY, robot_move_success, TOP_CANDLES_GOTO_NORMAL)
{
    robot->move.start (pg_cake_pos, Asserv::BACKWARD, pg_cake_radius
                       + pg_cake_distance + BOT_SIZE_SIDE);
}

FSM_TRANS (TOP_INIT, top_demo_candles, TOP_CANDLES_GOTO_NORMAL)
{
    robot->move.start (pg_cake_pos, Asserv::BACKWARD, pg_cake_radius
                       + pg_cake_distance + BOT_SIZE_SIDE);
}

FSM_TRANS (TOP_START, top_demo_follow, TOP_DEMO_FOLLOW)
{
    robot->asserv.follow (top.candles.dir_sign == 1
                          ? Asserv::FORWARD : Asserv::BACKWARD);
}

FSM_TRANS (TOP_DEMO_FOLLOW, top_follow_finished, TOP_DEMO_FOLLOW)
{
    // Transition needed for top_update.
}

