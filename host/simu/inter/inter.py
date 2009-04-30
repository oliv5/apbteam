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
"""Simulation interface."""
from simu.inter.drawable import *
from Tkinter import *

class TableView (DrawableCanvas):
    """This class handle the view of the table and every items inside it."""

    TABLE_WIDTH = 3000
    TABLE_HEIGHT = 2100
    MARGIN = 150

    def __init__ (self, master = None,
            width = TABLE_WIDTH, height = TABLE_HEIGHT):
        DrawableCanvas.__init__ (self,
                width + 2 * self.MARGIN, height + 2 * self.MARGIN,
                -width / 2, -height / 2,
                master, borderwidth = 1, relief = 'sunken',
                background = 'white')

class ActuatorView (DrawableCanvas):
    """This class handle the view of the actuators inside the robot."""

    UNIT = 120

    def __init__ (self, master = None):
        DrawableCanvas.__init__ (self, 1, 1, 0, 0, master,
                borderwidth = 1, relief = 'sunken', background = 'white')
        self.configure (width = self.UNIT, height = self.UNIT)
        self.size = 0.0

    def add_view (self, width = 1.0, height = 1.0):
        """Return a drawable suitable for an actuator view."""
        ratio = float (height) / float (width)
        self.size += ratio
        self.resize (1, self.size, 0, self.size / 2)
        self.configure (width = self.UNIT, height = self.UNIT * self.size)
        d = Drawable (self)
        d.trans_scale (1.0 / width)
        d.trans_translate ((0, - self.size + ratio / 2))
        return d

class Inter (Frame):
    """Robot simulation interface."""

    def __init__ (self, master = None):
        Frame.__init__ (self, master)
        self.pack (expand = True, fill = 'both')
        self.create_widgets ()

    def create_widgets (self):
        # Main layout.
        self.right_frame = Frame (self)
        self.right_frame.pack (side = 'right', fill = 'y')
        self.quit_button = Button (self.right_frame, text = 'Quit',
                command = self.quit)
        self.quit_button.pack (side = 'top')
        # Actuator view.
        self.actuator_view = ActuatorView (self.right_frame)
        self.actuator_view.pack (side = 'bottom', fill = 'x')
        # Sensor frame.
        self.sensor_frame = Frame (self.right_frame, borderwidth = 1,
                relief = 'sunken')
        self.sensor_frame.pack (side = 'bottom', fill = 'x')
        # Table view.
        self.table_view = TableView (self)
        self.table_view.pack (expand = True, fill = 'both')

    def update (self, *args):
        self.table_view.update ()
        self.actuator_view.update ()

if __name__ == '__main__':
    class TestTable (Drawable):
        def __init__ (self, onto, w, h):
            Drawable.__init__ (self, onto)
            self.w, self.h = w, h
        def draw (self):
            self.reset ()
            w, h = self.w, self.h
            self.draw_rectangle ((0, 0), (w, h), fill = 'blue')
            Drawable.draw (self)
    class TestRectangle (Drawable):
        def __init__ (self, onto, w, h, c1, c2):
            Drawable.__init__ (self, onto)
            self.w, self.h = 0.9 * w, 0.9 * h
            self.c1, self.c2 = c1, c2
        def draw (self):
            self.reset ()
            w, h = self.w, self.h
            self.draw_rectangle ((-w/2, -h/2), (w/2, h/2), fill = self.c1)
            self.draw_rectangle ((0, 0), (w/2, h/2), fill = self.c2)
            Drawable.draw (self)
    class TestSensor:
        def __init__ (self, master):
            self.button = Checkbutton (master, text = 'Sensor',
                    indicatoron = False)
            self.button.pack (side = 'top')
    app = Inter ()
    TestTable (app.table_view, 3000, 2100)
    TestRectangle (app.actuator_view.add_view (1, 1), 1, 1, 'red', 'green')
    TestRectangle (app.actuator_view.add_view (2, 1), 2, 1, 'green', 'blue')
    TestRectangle (app.actuator_view.add_view (1, 2), 1, 2, 'blue', 'red')
    TestSensor (app.sensor_frame)
    app.mainloop ()
