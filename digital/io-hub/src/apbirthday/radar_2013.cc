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
#include "radar_2013.hh"
#include "playground.hh"

RadarSensor sensors[] = {
    { 0, { 102, 84 }, G_ANGLE_UF016_DEG (0), true },
    { 0, { 102, -84 }, G_ANGLE_UF016_DEG (0), false },
    { 0, { -78, 104 }, G_ANGLE_UF016_DEG (180), true },
    { 0, { -83, -120 }, G_ANGLE_UF016_DEG (180), false },
};

Radar2013::Radar2013 (ucoo::UsDist &dist0, ucoo::UsDist &dist1,
                      ucoo::UsDist &dist2, ucoo::UsDist &dist3)
    : Radar (150, sensors, lengthof (sensors))
{
    sensors[0].sensor = &dist0;
    sensors[1].sensor = &dist1;
    sensors[2].sensor = &dist2;
    sensors[3].sensor = &dist3;
}

bool
Radar2013::valid (int sensor_index, vect_t &p)
{
    return p.x >= margin_mm && p.x < pg_width - margin_mm
        && p.y >= margin_mm && p.y < pg_length - margin_mm;
}

