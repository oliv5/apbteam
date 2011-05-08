# io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
#
# Copyright (C) 2011 Nicolas Schodet
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

    ELEVATION_STROKE = 0x3b0b

    ROTATION_STROKE = 0x11c6

    def __init__ (self, robot_class):
        TestSimu.__init__ (self, robot_class)

    def create_widgets (self):
        TestSimu.create_widgets (self)
        self.control_frame = Frame (self)
        self.control_frame.pack (side = 'left', before = self.table_view,
                fill = 'y')
        self.clamp_var = IntVar ()
        self.clamp_var.set (1)
        self.clamp_button = Checkbutton (self.control_frame, text = 'Clamp',
                indicatoron = False,
                variable = self.clamp_var, command = self.clamp_command)
        self.clamp_button.pack ()
        self.elevation_up_button = Button (self.control_frame,
                text = 'Elevation up', padx = 0, pady = 0,
                command = self.elevation_up_command)
        self.elevation_up_button.pack ()
        self.elevation_down_button = Button (self.control_frame,
                text = 'Elevation down', padx = 0, pady = 0,
                command = self.elevation_down_command)
        self.elevation_down_button.pack ()
        self.rotation_cw_button = Button (self.control_frame,
                text = 'Rotation cw', padx = 0, pady = 0,
                command = self.rotation_cw_command)
        self.rotation_cw_button.pack ()
        self.rotation_ccw_button = Button (self.control_frame,
                text = 'Rotation ccw', padx = 0, pady = 0,
                command = self.rotation_ccw_command)
        self.rotation_ccw_button.pack ()
        self.clamp_pos_scale = Scale (self.control_frame, orient = HORIZONTAL,
                from_ = 0, to = 6)
        self.clamp_pos_scale.pack ()
        self.clamp_move_button = Button (self.control_frame,
                text = 'Move clamp', padx = 0, pady = 0,
                command = self.clamp_move_command)
        self.clamp_move_button.pack ()
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
            self.io.pwm_set_timed (0, -0x3ff, 255, 0)
        else:
            self.io.pwm_set_timed (0, 0x3ff, 255, 0)

    def elevation_up_command (self):
        self.mimot.speed_pos ('a0', self.ELEVATION_STROKE / 2)

    def elevation_down_command (self):
        self.mimot.speed_pos ('a0', -self.ELEVATION_STROKE / 2)

    def rotation_cw_command (self):
        self.mimot.speed_pos ('a1', self.ROTATION_STROKE / 2)

    def rotation_ccw_command (self):
        self.mimot.speed_pos ('a1', -self.ROTATION_STROKE / 2)

    def clamp_move_command (self):
        self.io.clamp_move (self.clamp_pos_scale.get ())

    def change_color (self, *dummy):
        pass

if __name__ == '__main__':
    run ('robospierre', TestSimuControl)
