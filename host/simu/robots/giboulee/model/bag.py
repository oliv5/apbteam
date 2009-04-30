# simu - Robot simulation. {{{
#
# Copyright (C) 2009 Nicolas Schodet
#
# APBTeam:
#        Web: http://apbteam.org/
#      Email: team AT apbteam DOT org
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
# }}}
"""Giboulee bag of models."""
from simu.model.switch import Switch
from simu.model.position import Position
from simu.robots.giboulee.model.arm import Arm
from simu.robots.giboulee.model.sorter import Sorter

class Bag:

    def __init__ (self, link_bag):
        self.jack = Switch (link_bag.io.jack)
        self.color_switch = Switch (link_bag.io.color_switch)
        self.position = Position (link_bag.asserv.position)
        self.arm = Arm (link_bag.asserv.aux[0])
        self.sorter = Sorter (link_bag.io.servo[0:5], link_bag.io.servo[5])

