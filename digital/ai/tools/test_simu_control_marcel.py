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
from test_simu import TestSimu, run
from Tkinter import *
import math

class TestSimuControl (TestSimu):
    """Interface with extra control."""

    def __init__ (self, robot_class, *args):
        TestSimu.__init__ (self, robot_class, *args,
                color_switch_set_pos = True)
        self.io = self.robots[0].io
        self.asserv = self.robots[0].asserv
        self.mimot = self.robots[0].mimot
        self.robot_model = self.robots[0].model

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
        self.gate_var = IntVar ()
        self.gate_button = Checkbutton (self.control_frame,
                text = 'Gate', indicatoron = False,
                variable = self.gate_var, command = self.gate_command)
        self.gate_button.pack ()
        self.loader_up_button = Button (self.control_frame,
                text = 'Loader up', padx = 0, pady = 0,
                command = self.loader_up_command)
        self.loader_up_button.pack ()
        self.loader_down_button = Button (self.control_frame,
                text = 'Loader down', padx = 0, pady = 0,
                command = self.loader_down_command)
        self.loader_down_button.pack ()
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
            self.mimot.clamp ('a0', 0x60, 0x100)
            self.mimot.clamp ('a1', 0x60, 0x100)
        else:
            self.mimot.goto_pos ('a0', 0)
            self.mimot.goto_pos ('a1', 0)

    def elevator_command (self):
        if self.elevator_var.get ():
            pos = 7089
        else:
            pos = 0
        self.asserv.goto_pos ('a0', pos)

    def gate_command (self):
        if self.gate_var.get ():
            pos = -0x1d6b
        else:
            pos = 0
        self.asserv.goto_pos ('a1', pos)

    def loader_up_command (self):
        self.io.loader ('u')

    def loader_down_command (self):
        self.io.loader ('d')

if __name__ == '__main__':
    run ('marcel', TestSimuControl)
