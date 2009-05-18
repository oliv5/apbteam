# io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
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
from test_simu_aquajim import TestSimu
from Tkinter import *
import math

class TestSimuControl (TestSimu):
    """Interface with extra control."""

    def __init__ (self, asserv_cmd, io_cmd):
        TestSimu.__init__ (self, asserv_cmd, io_cmd)

    def create_widgets (self):
        TestSimu.create_widgets (self)
        self.control_frame = Frame (self)
        self.control_frame.pack (side = 'left', before = self.table_view,
                fill = 'y')
        self.cylinder_button = Button (self.control_frame, text = 'Cylinder',
                command = self.cylinder_command)
        self.cylinder_button.pack ()
        self.door_var = IntVar ()
        self.door_button = Checkbutton (self.control_frame, text = 'Door',
                indicatoron = False,
                variable = self.door_var, command = self.door_command)
        self.door_button.pack ()
        self.finger_var = IntVar ()
        self.finger_button = Checkbutton (self.control_frame, text = 'Finger',
                indicatoron = False,
                variable = self.finger_var, command = self.finger_command)
        self.finger_button.pack ()
        self.elevator_door_var = IntVar ()
        self.elevator_door_button = Checkbutton (self.control_frame,
                text = 'Elevator Door', indicatoron = False,
                variable = self.elevator_door_var,
                command = self.elevator_door_command)
        self.elevator_door_button.pack ()
        self.table_view.bind ('<1>', self.move)
        self.table_view.bind ('<3>', self.orient)

    def move (self, ev):
        pos = self.table_view.screen_coord ((ev.x, ev.y))
        self.asserv.goto (pos[0], pos[1])

    def orient (self, ev):
        x, y = self.table_view.screen_coord ((ev.x, ev.y))
        robot_pos = self.robot_model.position.pos
        if robot_pos is not None:
            a = math.atan2 (y - robot_pos[1], x - robot_pos[0])
            self.asserv.goto_angle (a)

    def cylinder_command (self):
        self.asserv.speed_pos ('a0', 5333 / 3)

    def door_command (self):
        door_pos = self.door_var.get ()
        self.io.servo_pos (0, door_pos)

    def finger_command (self):
        finger_pos = self.finger_var.get ()
        self.io.servo_pos (1, finger_pos)

    def elevator_door_command (self):
        elevator_door_pos = self.elevator_door_var.get ()
        self.io.pwm_set (elevator_door_pos == 1 and 512 or -512, 225)

    def change_color (self, *dummy):
        pass

if __name__ == '__main__':
    app = TestSimuControl (('../../asserv/src/asserv/asserv.host', '-m',
        'aquajim'), ('../src/io.host'))
    app.mainloop ()