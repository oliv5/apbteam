# simu - Robot simulation. {{{
#
# Copyright (C) 2013 Nicolas Schodet
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
"""APBirthday cake arm model."""
from utils.observable import Observable
from simu.utils.trans_matrix import TransMatrix

class CakeArm (Observable):

    far_x = 51
    far_y = 350
    near_x = 2
    near_y = 247

    def __init__ (self, table, robot_position, arm_cyl, far_cyl, near_cyl,
            arm_out_contact, arm_in_contact):
        Observable.__init__ (self)
        self.table = table
        self.robot_position = robot_position
        self.arm_cyl = arm_cyl
        self.far_cyl = far_cyl
        self.near_cyl = near_cyl
        self.arm_out_contact = arm_out_contact
        self.arm_in_contact = arm_in_contact
        self.far_pushed = False
        self.near_pushed = False
        self.arm_cyl.register (self.__arm_notified)
        self.far_cyl.register (self.__push_notified)
        self.near_cyl.register (self.__push_notified)
        self.robot_position.register (self.__robot_position_notified)

    def __arm_notified (self):
        if self.arm_cyl.pos > .9:
            contacts = (False, True)
        elif self.arm_cyl.pos < .1:
            contacts = (True, False)
        else:
            contacts = (True, True)
        for contact, state in ((self.arm_out_contact, contacts[0]),
                (self.arm_in_contact, contacts[1])):
            if contact.state != state:
                contact.state = state
                contact.notify ()
        self.notify ();

    def __push_notified (self):
        if self.arm_cyl.pos > .9:
            if not self.far_pushed and self.far_cyl.pos > .5:
                self.far_pushed = True
                self.__push (self.far_x, self.far_y, 3)
            elif self.far_pushed and self.far_cyl.pos < .9:
                self.far_pushed = False
            if not self.near_pushed and self.near_cyl.pos > .5:
                self.near_pushed = True
                self.__push (self.near_x, self.near_y, 2)
            elif self.near_pushed and self.near_cyl.pos < .9:
                self.near_pushed = False
        self.notify ()

    def __push (self, x, y, level):
        """Push a candle under coordinates x, y."""
        margin = 60
        # Matrix to transform an obstacle position into robot coordinates.
        m = self.__get_robot_matrix ()
        # Look up elements.
        for o in self.table.obstacles:
            if (o.level == level and hasattr (o, 'state') and o.state == False
                    and o.pos is not None):
                pos = m.apply (o.pos)
                if (pos[0] > x - margin
                        and pos[0] < x + margin
                        and pos[1] > y - margin
                        and pos[1] < y + margin):
                    o.state = True
                    o.notify ()

    def __robot_position_notified (self):
        # TODO: update color sensors.
        pass

    def __get_robot_matrix (self):
        """Return robot transformation matrix."""
        m = TransMatrix ()
        m.rotate (-self.robot_position.angle)
        m.translate ((-self.robot_position.pos[0],
            -self.robot_position.pos[1]))
        return m

