#ifndef radar_hh
#define radar_hh
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
#include "defs.hh"
#include "ucoolib/dev/usdist/usdist.hh"
#include "obstacles.hh"

/// Describe a radar sensor.
struct RadarSensor
{
    /// US distance sensor.
    ucoo::UsDist *sensor;
    /// Position relative to the robot center.
    vect_t pos;
    /// Angle relative to the robot X axis.
    uint16_t a;
    /// Part of an new sensor group?
    bool new_group;
};

/// Handle any distance sensors information to extract useful data.  This
/// includes:
///  - combining several sensors information for a more precise obstacle
///    position,
///  - ignoring obstacles not in the playground.
class Radar
{
  public:
    /// Margin to be considered inside the playground.  An obstacle can not be
    /// exactly at the playground edge.
    static const int margin_mm = 150;
  protected:
    /// Constructor.
    Radar (int obstacle_edge_radius_mm, RadarSensor *sensors, int sensors_nb)
        : obstacle_edge_radius_mm_ (obstacle_edge_radius_mm),
          sensors_ (sensors), sensors_nb_ (sensors_nb) { }
    /// Define exclusion area, return true if point is valid.
    virtual bool valid (int sensor_index, vect_t &p) = 0;
  public:
    /// Update radar view.  Record found obstacles.
    void update (const Position &robot_pos, Obstacles &obstacles);
  private:
    /// Estimated obstacle edge radius.  As the sensors detect obstacle edge,
    /// this is added to position obstacle center.
    const int obstacle_edge_radius_mm_;
    /// Sensors description.
    RadarSensor *sensors_;
    /// Number of sensors.
    int sensors_nb_;
};

#endif // radar_hh
