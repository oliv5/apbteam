# io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
#
# Copyright (C) 2012 Nicolas Schodet
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

import simu.robots.guybrush.model.bag

defs = simu.robots.guybrush.model.bag.Bag

class TestSimuControl (TestSimu):
    """Interface with extra control."""

    LOWER_CLAMP_ROTATION_STROKE = int (16 * 250)

    def __init__ (self, robot_class, *args):
        TestSimu.__init__ (self, robot_class, *args,
                color_switch_set_pos = True)
        self.io = self.robots[0].io
        self.asserv = self.robots[0].asserv
        self.mimot = self.robots[0].mimot
        self.robot_model = self.robots[0].model
        self.io.output (1 << defs.OUTPUT_UPPER_CLAMP_UP, 'toggle')
        self.io.output (1 << defs.OUTPUT_UPPER_CLAMP_IN, 'toggle')
        self.io.output (1 << defs.OUTPUT_DOOR_CLOSE, 'toggle')

    def create_widgets (self):
        TestSimu.create_widgets (self)
        self.control_frame = Frame (self)
        self.control_frame.pack (side = 'left', before = self.table_view,
                fill = 'y')
        Button (self.control_frame, text = 'FSM step', padx = 0, pady = 0,
                command = self.fsm_debug).pack ()
        def out_button (name, toggle):
            def command ():
                self.io.output (toggle, 'toggle')
            button = Button (self.control_frame, text = name,
                    padx = 0, pady = 0, command = command)
            button.pack ()
        out_button ('LClamp 1 open',
                1 << defs.OUTPUT_LOWER_CLAMP_1_CLOSE)
        out_button ('LClamp 2 open',
                1 << defs.OUTPUT_LOWER_CLAMP_2_CLOSE)
        self.lower_clamp_rotate_button = Button (self.control_frame,
                text = 'LClamp rotate', padx = 0, pady = 0,
                command = self.lower_clamp_rotate_command)
        self.lower_clamp_rotate_button.pack ()
        out_button ('UClamp up/down',
                1 << defs.OUTPUT_UPPER_CLAMP_UP
                | 1 << defs.OUTPUT_UPPER_CLAMP_DOWN)
        out_button ('UClamp in/out',
                1 << defs.OUTPUT_UPPER_CLAMP_IN
                | 1 << defs.OUTPUT_UPPER_CLAMP_OUT)
        out_button ('UClamp open',
                1 << defs.OUTPUT_UPPER_CLAMP_OPEN)
        out_button ('Door open',
                1 << defs.OUTPUT_DOOR_OPEN
                | 1 << defs.OUTPUT_DOOR_CLOSE)
        self.backward_var = IntVar ()
        self.backward_button = Checkbutton (self.control_frame,
                text = 'Backward', variable = self.backward_var)
        self.backward_button.pack ()
        self.goto_var = IntVar ()
        self.goto_button = Checkbutton (self.control_frame,
                text = 'Goto FSM', variable = self.goto_var)
        self.goto_button.pack ()
        self.table_view.bind ('<1>', self.move)
        self.table_view.bind ('<3>', self.orient)

    def fsm_debug (self):
        self.io.fsm_debug ()

    def move (self, ev):
        pos = self.table_view.screen_coord ((ev.x, ev.y))
        if self.goto_var.get ():
            self.io.goto (pos[0], pos[1], self.backward_var.get ())
        else:
            self.asserv.goto (pos[0], pos[1], self.backward_var.get ())

    def orient (self, ev):
        x, y = self.table_view.screen_coord ((ev.x, ev.y))
        robot_pos = self.robot_model.position.pos
        if robot_pos is not None:
            a = math.atan2 (y - robot_pos[1], x - robot_pos[0])
            self.asserv.goto_angle (a)

    def lower_clamp_rotate_command (self):
        self.mimot.speed_pos ('a0', self.LOWER_CLAMP_ROTATION_STROKE / 2)

if __name__ == '__main__':
    run ('guybrush', TestSimuControl)
