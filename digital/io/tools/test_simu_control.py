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
from test_simu import TestSimu
from Tkinter import *
import math

class TestSimuControl (TestSimu):
    """Interface with extra control."""

    def __init__ (self, asserv_cmd, mimot_cmd, io_cmd):
        TestSimu.__init__ (self, asserv_cmd, mimot_cmd, io_cmd)

    def create_widgets (self):
        TestSimu.create_widgets (self)
        self.control_frame = Frame (self)
        self.control_frame.pack (side = 'left', before = self.table_view,
                fill = 'y')
        self.clamp_var = IntVar ()
        self.clamp_button = Checkbutton (self.control_frame, text = 'Clamp',
                indicatoron = False,
                variable = self.clamp_var, command = self.clamp_command)
        self.clamp_button.pack ()
        self.elevator_var = IntVar ()
        self.elevator_button = Checkbutton (self.control_frame,
                text = 'Elevator', indicatoron = False,
                variable = self.elevator_var, command = self.elevator_command)
        self.elevator_button.pack ()
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

    def clamp_command (self):
        if self.clamp_var.get ():
            pos = 1720
        else:
            pos = 0
        self.mimot.goto_pos ('a0', pos)
        self.mimot.goto_pos ('a1', pos)

    def elevator_command (self):
        if self.elevator_var.get ():
            pos = 7089
        else:
            pos = 0
        self.asserv.goto_pos ('a0', pos)

    def change_color (self, *dummy):
        pass

if __name__ == '__main__':
    app = TestSimuControl (('../../asserv/src/asserv/asserv.host', '-m9',
        'marcel'),
        ('../../mimot/src/dirty/dirty.host', '-m9', 'marcel'),
        ('../src/io.host'))
    app.mainloop ()
    app.close ()
