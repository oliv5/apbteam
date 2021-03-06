# inter - Robot simulation interface. {{{
#
# Copyright (C) 2008 Nicolas Schodet
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
"""Inter and its childrens."""
from Tkinter import *
from simu.inter.drawable import *

from math import pi, cos, sin

class Robot (Drawable):
    """The robot."""

    def draw (self):
        self.reset ()
        if self.angle is not None:
            self.trans_translate (self.pos)
            self.trans_rotate (self.angle)
            self.draw_polygon ((115, 30), (170, 85), (150, 127), (130, 145),
                    (-25, 200), (-70, 200), (-70, -200), (-25, -200),
                    (130, -145), (150, -127), (170, -85), (115, -30))
            axes_fill = '#404040'
            self.draw_line ((-70, 0), (170, 0), fill = axes_fill, arrow = LAST)
            f = 142 + 2 * 31.5 - 13.5
            wr = 63 / 2
            self.draw_line ((0, +f / 2), (0, -f / 2), fill = axes_fill)
            self.draw_line ((-wr, f / 2), (+wr, f / 2), fill = axes_fill)
            self.draw_line ((-wr, -f / 2), (+wr, -f / 2), fill = axes_fill)
            Drawable.draw (self)

class Obstacle (Drawable):
    """An obstacle."""

    def __init__ (self, onto, pos, radius):
        Drawable.__init__ (self, onto)
        self.pos = pos
        self.radius = radius

    def draw (self):
        self.reset ()
        self.trans_translate (self.pos)
        self.draw_circle ((0, 0), self.radius, fill = '#31aa23')
        self.draw_circle ((0, 0), self.radius + 250, outlinestipple = 'gray25')
        Drawable.draw (self)

class Table (Drawable):
    """The table and its elements."""

    def draw (self):
        # Redraw.
        self.reset ()
        # Table.
        self.draw_rectangle ((-22, -22 - 80), (3000 / 2, 2100 + 22), fill = '#ff1f1f')
        self.draw_rectangle ((3000 / 2, -22 - 80), (3000 + 22, 2100 + 22), fill = '#201fff')
        self.draw_rectangle ((0, 0), (3000, 2100), fill = '#a49d8b')
        self.draw_rectangle ((0, -22 - 80), (3000, -22), fill = '#a49d8b')
        self.draw_rectangle ((0, 2100 - 500), (500, 2100), fill = '#201fff')
        self.draw_rectangle ((3000 - 500, 2100 - 500), (3000, 2100), fill = '#ff1f1f')
        self.draw_line ((3000 / 2, -22 - 80), (3000 / 2, 2100 + 22))
        # Axes.
        self.draw_line ((0, 200), (0, 0), (200, 0), arrow = BOTH)
        # Beacons and baskets.
        self.draw_rectangle ((-22, 2100), (-22 - 80, 2100 + 80), fill = '#5b5b5d')
        self.draw_rectangle ((-22, 1050 - 40), (-22 - 80, 1050 + 40), fill = '#5b5b5d')
        self.draw_rectangle ((-22, 500), (-22 - 80, 500 + 80), fill = '#5b5b5d')
        self.draw_rectangle ((-22, -80), (-22 - 80, 0), fill = '#5b5b5d')
        self.draw_rectangle ((-22, 0), (-22 - 80, 500), fill = '#5b5b5d')
        self.draw_rectangle ((-22 - 80 - 250, 0), (-22 - 80, 500), fill = '#6d6dad', stipple = 'gray75')
        self.draw_rectangle ((3000 + 22, 2100), (3000 + 22 + 80, 2100 + 80), fill = '#5b5b5d')
        self.draw_rectangle ((3000 + 22, 1050 - 40), (3000 + 22 + 80, 1050 + 40), fill = '#5b5b5d')
        self.draw_rectangle ((3000 + 22, 500), (3000 + 22 + 80, 500 + 80), fill = '#5b5b5d')
        self.draw_rectangle ((3000 + 22, -80), (3000 + 22 + 80, 0), fill = '#5b5b5d')
        self.draw_rectangle ((3000 + 22, 0), (3000 + 22 + 80, 500), fill = '#5b5b5d')
        self.draw_rectangle ((3000 + 22 + 80 + 250, 0), (3000 + 22 + 80, 500), fill = '#6d6dad', stipple = 'gray75')
        # Vertical dispensers.
        self.draw_rectangle ((-22, 2100 - 750 - 85 / 2), (0, 2100 - 750 + 85 / 2), fill = '#5b5b5b')
        self.draw_circle ((40, 2100 - 750), 40)
        self.draw_rectangle ((700 - 85 / 2, 2100), (700 + 85 / 2, 2100 + 22), fill = '#5b5b5b')
        self.draw_circle ((700, 2100 - 40), 40)
        self.draw_rectangle ((3000 + 22, 2100 - 750 - 85 / 2), (3000, 2100 - 750 + 85 / 2), fill = '#5b5b5b')
        self.draw_circle ((3000 - 40, 2100 - 750), 40)
        self.draw_rectangle ((3000 - 700 + 85 / 2, 2100), (3000 - 700 - 85 / 2, 2100 + 22), fill = '#5b5b5b')
        self.draw_circle ((3000 - 700, 2100 - 40), 40)
        # Horizontal dispenser.
        self.draw_rectangle ((3000 / 2 - 924 / 2, 2100 + 22), (3000 / 2 + 924 / 2, 2100 + 22 + 80 + 22), fill = '#5b5b5b')
        self.draw_rectangle ((3000 / 2 - 924 / 2 + 22, 2100 + 22), (3000 / 2 + 924 / 2 - 22, 2100 + 22 + 80), fill = '#5b5b5b')
        self.draw_rectangle ((3000 / 2 - 880 / 2 - 35 - 60, 2100), (3000 / 2 - 880 / 2 - 35, 2100 + 44), fill = '#5b5b5b')
        self.draw_rectangle ((3000 / 2 + 880 / 2 + 35 + 60, 2100), (3000 / 2 + 880 / 2 + 35, 2100 + 44), fill = '#5b5b5b')
        # Balls.
        balls = [ (800, 200, 'rb'), (800, 400, 'RB'), (800, 600, 'ww'),
                (1300, 200, 'rb'), (1300, 400, 'rb'), (1300, 600, 'ww'),
                (520, 800, 'WW'), (700, 40, 'RB'), (40, 750, 'WW'),
                (450, 1120, 'ww'), (750, 1070, 'ww'), (1050, 1020, 'ww'),
                (1500 - 72 / 2, -22 - 80 / 2, 'BR'),
                (1500 - 72 / 2 - 1 * 73, -22 - 80 / 2, 'RB'),
                (1500 - 72 / 2 - 2 * 73, -22 - 80 / 2, 'BR'),
                (1500 - 72 / 2 - 3 * 73, -22 - 80 / 2, 'RB'),
                (1500 - 72 / 2 - 4 * 73, -22 - 80 / 2, 'BR'),
                (1500 - 72 / 2 - 5 * 73, -22 - 80 / 2, 'RB'),
                (1500, 1000, 'W'),
                ]
        balls_config = { 'r': { 'outline': '#bf4141' }, 'R': { 'fill': '#bf4141' },
                'b': { 'outline': '#4241bf' }, 'B': { 'fill': '#4241bf' },
                'w': { 'outline': '#bfbfbf' }, 'W': { 'fill': '#bfbfbf' } }
        for b in balls:
            self.draw_circle ((3000 - b[0], 2100 - b[1]), 72 / 2,
                    **balls_config[b[2][0]])
            if len (b[2]) > 1:
                self.draw_circle ((b[0], 2100 - b[1]), 72 / 2,
                        **balls_config[b[2][1]])
        Drawable.draw (self)

class TableView (DrawableCanvas):
    """This class handle the view of the table and every items inside it."""

    WIDTH = 3000 + 2 * 22 + 2 * 80 + 2 * 50 + 2 * 10
    HEIGHT = 2100 + 2 * 22 + 2 * 80 + 22 + 2 * 10
    XORIGIN = -3000 / 2
    YORIGIN = -2100 / 2

    def __init__ (self, master = None):
        DrawableCanvas.__init__ (self, self.WIDTH, self.HEIGHT, self.XORIGIN,
                self.YORIGIN,
                master, borderwidth = 1, relief = 'sunken',
                background = 'white')
        self.table = Table (self)
        self.robot = Robot (self.table)
        self.robot.angle = 0
        self.robot.pos = (0, 0)

class Arm (Drawable):
    """The robot arm."""

    def draw (self):
        self.reset ()
        self.draw_arc ((0, 0), 0.45, start = 7 * pi / 12, extent = 10 * pi / 12,
                style = 'arc', outline = '#808080')
        self.draw_arc ((0, 0), 0.45, start = -5 * pi / 12, extent = 10 * pi / 12,
                style = 'arc', outline = '#808080')
        self.draw_arc ((0, 0), 0.25, start = -7 * pi / 12, extent = 14 * pi / 12,
                style = 'arc', outline = '#808080')
        self.trans_scale (0.4)
        self.trans_rotate (-self.angle)
        self.draw_line ((0, 0), (0, 1))
        self.draw_line ((0, 1), (0.3, 1), arrow = LAST, fill = '#808080')
        self.draw_line ((0, 0), (cos (pi / 6), -sin (pi / 6)))
        self.draw_line ((0, 0), (-cos (pi / 6), -sin (pi / 6)))
        Drawable.draw (self)

class Servo (Drawable):
    """Servo motor."""

    def __init__ (self, onto, coord, l, start, extent):
        Drawable.__init__ (self, onto)
        self.coord = coord
        self.l = l
        self.start = start
        self.extent = extent
        self.pos = 0

    def draw (self):
        self.reset ()
        self.draw_arc (self.coord, self.l, start = self.start,
                extent = self.extent, style = 'arc', outline = '#808080')
        a = self.start + self.pos * self.extent
        self.draw_line (self.coord, (self.coord[0] + self.l * cos (a),
            self.coord[1] + self.l * sin (a)))
        Drawable.draw (self)

class Rear (Drawable):
    """Rear actuators."""

    def __init__ (self, onto):
        Drawable.__init__ (self, onto)
        self.traps = [
                Servo (self, (-2.5, -1), 0.8, 0, pi/2),
                Servo (self, (-1.5, -0.9), 0.8, 0, pi/2),
                Servo (self, (-0.5, -0.8), 0.8, 0, pi/2),
                Servo (self, (0.5, -0.8), 0.8, pi, -pi/2),
                Servo (self, (1.5, -0.9), 0.8, pi, -pi/2),
                Servo (self, (-2.5, 1.3), 0.8, -pi/6, pi/3),
                ]

    def draw (self):
        self.reset ()
        self.trans_scale (0.9/5)
        self.draw_line ((-0.5, 1.5), (-0.5, 0.5), (-2.5, 0.2),
                fill = '#808080')
        self.draw_line ((-2.5, -1.2), (-2.5, -2.3), (2.5, -2.3), (2.5, 0.2),
                (0.5, 0.5), (0.5, 1.5), fill = '#808080')
        for i in (-1.5, -0.5, 0.5, 1.5):
            self.draw_line ((i, -2.3), (i, -2), fill = '#808080')
        Drawable.draw (self)

class ActuatorView (DrawableCanvas):
    """This class handle the view of the actuators inside the robot."""

    def __init__ (self, master = None):
        DrawableCanvas.__init__ (self, 1, 2, 0, 0, master,
                borderwidth = 1, relief = 'sunken', background = 'white')
        self.configure (width = 120, height = 240)
        self.arm_drawable = Drawable (self)
        self.arm_drawable.trans_translate ((0, 0.5))
        self.arm = Arm (self.arm_drawable)
        self.arm.angle = 0
        self.rear_drawable = Drawable (self)
        self.rear_drawable.trans_translate ((0, -0.5))
        self.rear = Rear (self.rear_drawable)

class Inter (Frame):
    """Robot simulation interface."""

    def __init__ (self, master = None):
        Frame.__init__ (self, master)
        self.pack (expand = 1, fill = 'both')
        self.createWidgets ()

    def createWidgets (self):
        self.rightFrame = Frame (self)
        self.rightFrame.pack (side = 'right', fill = 'y')
        self.quitButton = Button (self.rightFrame, text = 'Quit', command = self.quit)
        self.quitButton.pack (side = 'top')
        self.actuatorview = ActuatorView (self.rightFrame)
        self.actuatorview.pack (side = 'bottom', fill = 'x')

        self.sensorFrame = Frame (self.rightFrame, borderwidth = 1, relief =
                'sunken')
        self.sensorFrame.pack (side = 'bottom', fill = 'x')
        self.jackVar = IntVar ()
        self.jackButton = Checkbutton (self.sensorFrame, text = 'Jack',
                indicatoron = False, variable = self.jackVar)
        self.jackButton.pack (side = 'top')
        self.colorVar = IntVar ()
        self.colorButton = Checkbutton (self.sensorFrame, text = 'Color',
                indicatoron = False, variable = self.colorVar)
        self.colorButton.pack (side = 'top')

        self.tableview = TableView (self)
        self.tableview.pack (expand = True, fill = 'both')

    def update (self, *args):
        """Redraw all objects to be updated."""
        self.tableview.update ()
        self.actuatorview.update ()

if __name__ == '__main__':
    app = Inter ()
    app.tableview.robot.angle = pi / 3
    app.tableview.robot.pos = (700, 700)
    app.tableview.robot.update ()
    if 0:
        from dist_sensor import DistSensor
        ds = DistSensor (app.tableview.robot, (150, -127), -pi / 12, 800)
        obs = Obstacle (app.tableview, (1300, 1200), 150)
        ds.obstacles = [ obs ]
    app.actuatorview.arm.angle = pi/6
    app.actuatorview.arm.update ()
    app.actuatorview.rear.traps[0].pos = 1
    app.actuatorview.rear.traps[1].pos = 0
    app.actuatorview.rear.traps[2].pos = 0
    app.actuatorview.rear.traps[3].pos = 1
    app.actuatorview.rear.traps[4].pos = 0
    app.actuatorview.rear.traps[5].pos = 0
    app.actuatorview.rear.update ()
    app.mainloop()
