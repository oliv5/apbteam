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
#include "move.hh"
#include "robot.hh"
#include "bot.hh"
#include "playground.hh"

#include <cstdlib>
#include <cmath>

void
Move::start (const Position &position,
             Asserv::DirectionConsign direction_consign,
             int shorten)
{
    // Set parameters.
    final_ = position;
    with_angle_ = true;
    direction_consign_ = direction_consign;
    shorten_ = shorten;
    // Start the FSM.
    robot->fsm_queue.post (FSM_EVENT (move_start));
}

void
Move::start (const vect_t &position,
             Asserv::DirectionConsign direction_consign,
             int shorten)
{
    // Set parameters.
    final_.v = position;
    with_angle_ = false;
    direction_consign_ = direction_consign;
    shorten_ = shorten;
    // Start the FSM.
    robot->fsm_queue.post (FSM_EVENT (move_start));
}

void
Move::stop ()
{
    // Stop the FSM.
    robot->fsm_queue.post (FSM_EVENT (move_stop));
}

bool
Move::check_obstacles ()
{
    if (FSM_CAN_HANDLE (AI, obstacle_blocking))
    {
        Position robot_pos;
        robot->asserv.get_position (robot_pos);
        if (robot->obstacles.blocking (robot_pos.v, step_))
            if (FSM_HANDLE (AI, obstacle_blocking))
                return true;
    }
    return false;
}

FSM_STATES (
	    // Waiting for the start order.
	    MOVE_IDLE,
	    // Rotating towards next point.
	    MOVE_ROTATING,
	    // Moving to a position (intermediate or final).
	    MOVE_MOVING,
	    // Brake when a obstacle is seen.
	    MOVE_BRAKE,
	    // Moving backward to go away from what is blocking the bot.
	    MOVE_MOVING_BACK_UP)

FSM_EVENTS (
	    // Report from asserv after a successful move command.
	    robot_move_success,
	    // Report from asserv after a failed move command.
	    robot_move_failure,
	    // Initialize the FSM and start the movement directly.
	    move_start,
	    // Stop movement.
	    move_stop,
	    // Movement success.
	    move_success,
	    // Movement failure.
	    move_failure,
	    // There is an obstacle blocking on path.
	    obstacle_blocking)

FSM_START_WITH (MOVE_IDLE)

void
Move::go ()
{
    vect_t dst = step_;
    // Modify final point if requested.
    if (step_final_move_ && shorten_)
    {
        // Compute a vector from destination to robot with lenght 'shorten'.
        Position robot_position;
        robot->asserv.get_position (robot_position);
        vect_t v = robot_position.v;
        vect_sub (&v, &step_);
        int d = vect_norm (&v);
        if (d > shorten_)
        {
            vect_scale_f824 (&v, 0x1000000 / d * shorten_);
            vect_translate (&dst, &v);
        }
    }
    if (step_with_angle_)
    {
        Position dst_pos = { dst, step_angle_ };
        robot->asserv.goto_xya (dst_pos, step_direction_consign_);
    }
    else
        robot->asserv.goto_xy (dst, step_direction_consign_);
}

Move::NextMove
Move::go_or_rotate (const vect_t &dst, uint16_t angle, bool with_angle,
                    Asserv::DirectionConsign direction_consign)
{
    Position robot_position;
    robot->asserv.get_position (robot_position);
    uint16_t robot_angle = robot_position.a;
    if (direction_consign & Asserv::BACKWARD)
        robot_angle += 0x8000;
    // Remember step.
    step_ = dst;
    step_angle_ = angle;
    step_with_angle_ = with_angle;
    step_direction_consign_ = direction_consign;
    // Compute angle to destination.
    vect_t v = dst; vect_sub (&v, &robot_position.v);
    uint16_t dst_angle = std::atan2 (v.y, v.x) * ((1l << 16) / (2 * M_PI));
    if (direction_consign & Asserv::BACKWARD)
        dst_angle += 0x8000;
    int16_t diff_angle = dst_angle - robot_angle;
    if ((direction_consign & Asserv::REVERT_OK)
        && (diff_angle > 0x4000 || diff_angle < -0x4000))
        dst_angle += 0x8000;
    int16_t diff = dst_angle - robot_angle;
    // Move or rotate.
    if (std::abs (diff) < 0x1000)
    {
        go ();
        return LINEAR;
    }
    else
    {
        robot->asserv.goto_angle (dst_angle);
        return ANGULAR;
    }
}

Move::NextMove
Move::go_to_next (const vect_t &dst)
{
    NextMove r;
    // If it is not the last position.
    if (dst.x != final_.v.x || dst.y != final_.v.y)
    {
        // Not final position.
        step_final_move_ = false;
        // Goto without angle.
        r = go_or_rotate (dst, 0, false,
                          Asserv::DirectionConsign (direction_consign_
                                                    | (slow_ ? Asserv::REVERT_OK : 0)));
    }
    else
    {
        // Final position.
        step_final_move_ = true;
        // Goto with angle if requested.
        r = go_or_rotate (dst, final_.a, with_angle_, direction_consign_);
    }
    // Next time, do not use slow.
    slow_ = false;
    return r;
}

Move::NextMove
Move::path_init ()
{
    bool found;
    vect_t dst;
    // Get the current position
    Position current_pos;
    robot->asserv.get_position (current_pos);
    // Give the current position of the bot to the path module
    robot->path.reset ();
    robot->obstacles.add_obstacles (robot->path);
    robot->path.endpoints (current_pos.v, final_.v);
    // Update the path module
    slow_ = 0;
    robot->path.compute ();
    found = robot->path.get_next (dst);
    // If not found, try to escape.
    if (!found)
    {
        slow_ = 1;
        robot->path.compute (8);
        found = robot->path.get_next (dst);
    }
    // If found, go.
    if (found)
        return go_to_next (dst);
    else
        return NONE;
}

Move::NextMove
Move::path_next ()
{
    vect_t dst;
    robot->path.get_next (dst);
    return go_to_next (dst);
}

FSM_TRANS (MOVE_IDLE, move_start,
	   path_found_rotate, MOVE_ROTATING,
	   path_found, MOVE_MOVING,
	   no_path_found, MOVE_IDLE)
{
    switch (robot->move.path_init ())
    {
    default:
    case Move::NONE:
        robot->fsm_queue.post (FSM_EVENT (move_failure));
        return FSM_BRANCH (no_path_found);
    case Move::LINEAR:
        return FSM_BRANCH (path_found);
    case Move::ANGULAR:
        return FSM_BRANCH (path_found_rotate);
    }
}

FSM_TRANS (MOVE_ROTATING, robot_move_success, MOVE_MOVING)
{
    robot->move.go ();
}

FSM_TRANS (MOVE_ROTATING, robot_move_failure, MOVE_MOVING)
{
    robot->move.go ();
}

FSM_TRANS_TIMEOUT (MOVE_ROTATING, 1250, MOVE_MOVING)
{
    robot->move.go ();
}

FSM_TRANS (MOVE_ROTATING, move_stop, MOVE_IDLE)
{
    robot->asserv.stop ();
}

FSM_TRANS (MOVE_MOVING, robot_move_success,
	   done, MOVE_IDLE,
	   path_found_rotate, MOVE_ROTATING,
	   path_found, MOVE_MOVING)
{
    if (robot->move.step_final_move_)
    {
        robot->fsm_queue.post (FSM_EVENT (move_success));
        return FSM_BRANCH (done);
    }
    else
    {
        if (robot->move.path_next () == Move::LINEAR)
            return FSM_BRANCH (path_found);
        else
            return FSM_BRANCH (path_found_rotate);
    }
}

/// Test if a point is ok to move back.
static bool
move_test_point (const vect_t &p, Direction direction)
{
    int margin = BOT_SIZE_RADIUS;
    return p.x >= margin && p.x < pg_width - margin
        && p.y >= margin && p.y < pg_length - margin;
    // TODO: cake.
}

static void
move_back_up (void)
{
    int dist, back_dist;
    Direction dir = robot->asserv.get_last_moving_direction ();
    if (dir == DIRECTION_FORWARD)
    {
        dist = BOT_SIZE_FRONT + Obstacles::obstacle_radius_mm;
        back_dist = -300;
    }
    else
    {
        dist = -(BOT_SIZE_BACK + Obstacles::obstacle_radius_mm);
        back_dist = 300;
    }
    // Assume there is an obstacle in front of the robot.
    Position robot_pos;
    robot->asserv.get_position (robot_pos);
    vect_t obstacle_pos;
    vect_from_polar_uf016 (&obstacle_pos, dist, robot_pos.a);
    vect_translate (&obstacle_pos, &robot_pos.v);
    robot->obstacles.add (obstacle_pos);
    // Move backward to turn freely.
    vect_t back_pos;
    vect_from_polar_uf016 (&back_pos, back_dist, robot_pos.a);
    vect_translate (&back_pos, &robot_pos.v);
    if (!move_test_point (back_pos, dir))
        back_dist /= 8;
    robot->asserv.move_distance (back_dist);
}

FSM_TRANS (MOVE_MOVING, robot_move_failure, MOVE_MOVING_BACK_UP)
{
    move_back_up ();
}

FSM_TRANS_TIMEOUT (MOVE_MOVING, 2500, MOVE_MOVING_BACK_UP)
{
    move_back_up ();
}

FSM_TRANS (MOVE_MOVING, obstacle_blocking, MOVE_BRAKE)
{
    robot->asserv.stop ();
}

FSM_TRANS (MOVE_MOVING, move_stop, MOVE_IDLE)
{
    robot->asserv.stop ();
}

FSM_TRANS (MOVE_BRAKE, robot_move_success, MOVE_IDLE)
{
    robot->fsm_queue.post (FSM_EVENT (move_failure));
}

FSM_TRANS (MOVE_BRAKE, robot_move_failure, MOVE_IDLE)
{
    robot->fsm_queue.post (FSM_EVENT (move_failure));
}

FSM_TRANS (MOVE_BRAKE, move_stop, MOVE_IDLE)
{
}

FSM_TRANS (MOVE_MOVING_BACK_UP, robot_move_success, MOVE_IDLE)
{
    robot->fsm_queue.post (FSM_EVENT (move_failure));
}

FSM_TRANS (MOVE_MOVING_BACK_UP, robot_move_failure, MOVE_IDLE)
{
    robot->fsm_queue.post (FSM_EVENT (move_failure));
}

FSM_TRANS (MOVE_MOVING_BACK_UP, move_stop, MOVE_IDLE)
{
    robot->asserv.stop ();
}

