#ifndef mimot_hh
#define mimot_hh
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
#include "motor.hh"

/// Interface to mimot board.
class Mimot : public I2cQueue::Slave
{
  public:
    static const int aux_nb = 2;
  public:
    /// Constructor.
    Mimot (I2cQueue &queue);
    /// See I2cQueue::Slave::recv_status.
    void recv_status (const uint8_t *status);
  public:
    /// Mimot single output.
    class Motor
    {
      public:
        /// Maximum positive value for PWM.
        static const int16_t pwm_max = 0x3ff;
      public:
        /// Contructor.
        Motor (Mimot &mimot, int index) : mimot_ (mimot), index_ (index) { }
        /// Get last command status.
        ::Motor::Status get_status () const;
        /// Get current shaft position.
        uint16_t get_position () const;
        /// Move to absolute position at given speed.
        void move (uint16_t position, uint16_t speed);
        /// Set motor to free running, no torque.
        void free ();
        /// Set raw output.
        void output_set (int16_t pwm);
      private:
        Mimot &mimot_;
        int index_;
        uint16_t position_;
        friend class Mimot;
    };
    friend class Motor;
    /// Motors public access.
    Motor motor0, motor1;
  private:
    uint8_t status_flag_;
    uint8_t input_port_;
};

inline Motor::Status
Mimot::Motor::get_status () const
{
    uint8_t status = mimot_.status_flag_ >> (index_ * 2);
    if (status & 1)
        return ::Motor::SUCCESS;
    else if (status & 2)
        return ::Motor::FAILURE;
    else
        return ::Motor::RUNNING;
}

inline uint16_t
Mimot::Motor::get_position () const
{
    return position_;
}

#endif // mimot_hh
