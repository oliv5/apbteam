#ifndef asserv_hh
#define asserv_hh
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
#include "i2c_queue.hh"
#include "defs.hh"
#include "motor.hh"

/// Interface to asserv board.
class Asserv : public I2cQueue::Slave
{
  public:
    /// No support for auxiliary motors for the moment.
    static const int aux_nb = 0;
  public:
    /// Direction consigns, can be or'ed together.
    enum DirectionConsign
    {
        /// Use forward movements.
        FORWARD = 0,
        /// Use backward movements.
        BACKWARD = 1,
        /// Authorise to reverse the direction consign
        REVERT_OK = 2,
        /// Combination.
        BACKWARD_REVERT_OK = BACKWARD | REVERT_OK,
    };
  public:
    /// Constructor.
    Asserv (I2cQueue &queue, float scale);
    /// See I2cQueue::Slave::recv_status.
    void recv_status (const uint8_t *status);
    /// Get status of last command.
    Motor::Status get_status () const;
    /// Get current moving direction (or NONE).
    Direction get_moving_direction () const;
    /// Get last moving direction.
    Direction get_last_moving_direction () const;
    /// Get current position.
    void get_position (Position &position) const;
    /// Release motors (zero torque).
    void free ();
    /// Stop moving, applying acceleration constraints.
    void stop ();
    /// Move linearly on a given distance (mm).
    void move_distance (int distance);
    /// Move angularly by a given angle.
    void move_angle (int16_t angle);
    /// Face an absolute angle.
    void goto_angle (uint16_t angle);
    /// Go to an absolute position and angle.
    void goto_xya (const Position &pos,
                   DirectionConsign direction_consign = FORWARD);
    /// Go to an absolute position, any angle.
    void goto_xy (const vect_t &vect,
                  DirectionConsign direction_consign = FORWARD);
    /// Push the wall and initialise position. Use -1 for coordinates to keep
    /// unchanged.
    void push_wall (DirectionConsign direction_consign,
                    int init_x, int init_y, int16_t init_a);
    /// Set movement speeds.
    void set_speed (uint16_t linear_hi, uint16_t angular_hi,
                    uint16_t linear_lo, uint16_t angular_lo);
    /// Set acceleration.
    void set_acceleration (uint16_t linear, uint16_t angular);
  private:
    uint8_t status_flag_;
    uint8_t input_port_;
    int position_x_, position_y_;
    uint16_t position_a_;
    Direction last_moving_direction_;
    float scale_, scale_inv_;
};

inline Motor::Status
Asserv::get_status () const
{
    if (status_flag_ & 1)
        return Motor::SUCCESS;
    else if (status_flag_ & 2)
        return Motor::FAILURE;
    else
        return Motor::RUNNING;
}

inline Direction
Asserv::get_moving_direction () const
{
    if (status_flag_ & 4)
        return DIRECTION_FORWARD;
    else if (status_flag_ & 8)
        return DIRECTION_BACKWARD;
    else
        return DIRECTION_NONE;
}

inline Direction
Asserv::get_last_moving_direction () const
{
    return last_moving_direction_;
}

#endif // asserv_hh
