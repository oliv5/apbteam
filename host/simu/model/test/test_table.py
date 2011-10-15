# simu - Robot simulation. {{{
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
"""Test table model and obstacles."""
from simu.model.table import Table
from simu.model.round_obstacle import RoundObstacle
from simu.inter.drawable import Drawable, DrawableCanvas
from simu.utils.vector import vector
from Tkinter import *

class Area (Drawable):

    def __init__ (self, onto):
        Drawable.__init__ (self, onto)
        self.table = Table ()
        self.a = (0, 0)
        self.b = (10, 10)
        self.result = None

    def draw (self):
        self.reset ()
        for o in self.table.obstacles:
            if o.pos is None:
                continue
            if isinstance (o, RoundObstacle):
                self.draw_circle (o.pos, o.radius)
            else:
                raise TypeError ("unknown obstacle")
        if self.a is not None:
            self.draw_circle (self.a, 0.2, fill = 'green')
        if self.b is not None:
            self.draw_circle (self.b, 0.2, fill = 'red')
        if self.a is not None and self.b is not None:
            self.draw_line (self.a, self.b, arrow = 'last')
        if self.result is not None:
            self.draw_circle (self.result, 0.2, fill = 'yellow')

    def update (self, test, **kwargs):
        self.result = None
        if self.a is not None and self.b is not None:
            if test == 'intersect':
                def nearer (a, b): return a < b
                i = self.table.intersect (self.a, self.b, comp = nearer,
                        **kwargs)
                if i is not None:
                    a, b = vector (self.a), vector (self.b)
                    self.result = (b - a).unit () * i.distance
            elif test == 'nearest':
                n = self.table.nearest (self.b, **kwargs)
                if n is not None:
                    self.result = n.pos

class AreaView (DrawableCanvas):

    def __init__ (self, master = None):
        DrawableCanvas.__init__ (self, 22, 22, 0, 0, master, borderwidth = 1,
                relief = 'sunken', background = 'white')
        self.area = Area (self)

    def draw (self):
        self.area.draw ()

class TestTable (Frame):

    def __init__ (self, master = None):
        Frame.__init__ (self, master)
        self.pack (expand = True, fill = 'both')
        self.create_widgets ()
        self.move = None

    def create_widgets (self):
        self.right_frame = Frame (self)
        self.right_frame.pack (side = 'right', fill = 'y')
        self.quit_button = Button (self.right_frame, text = 'Quit', command =
                self.quit)
        self.quit_button.pack (side = 'top', fill = 'x')
        self.test_var = StringVar ()
        self.test_var.set ('intersect')
        self.test_intersect = Radiobutton (self.right_frame,
                variable = self.test_var, command = self.update,
                text = 'Intersect', value = 'intersect')
        self.test_intersect.pack (side = 'top')
        self.test_nearest = Radiobutton (self.right_frame,
                variable = self.test_var, command = self.update,
                text = 'Nearest', value = 'nearest')
        self.test_nearest.pack (side = 'top')
        self.new_obstacle_round_frame = LabelFrame (self.right_frame)
        self.new_obstacle_round_frame.pack (side = 'top')
        self.new_obstacle_radius = Scale (self.new_obstacle_round_frame,
                label = 'Radius', orient = 'horizontal', from_ = 0.5, to = 10,
                resolution = 0.5)
        self.new_obstacle_radius.pack (side = 'top')
        self.new_obstacle_round = Button (self.new_obstacle_round_frame,
                text = 'New round obstacle',
                command = self.new_round_obstacle)
        self.new_obstacle_round_frame.configure (
                labelwidget = self.new_obstacle_round)
        self.area_view = AreaView (self)
        self.area_view.pack (expand = True, fill = 'both')
        self.area_view.bind ('<1>', self.click)

    def update (self, draw = True):
        self.area_view.area.update (self.test_var.get ())
        if draw:
            self.area_view.draw ()

    def click (self, ev):
        def move (o, pos):
            if callable (o):
                o (pos)
            else:
                o.pos = pos
        pos = vector (self.area_view.screen_coord ((ev.x, ev.y)))
        if self.move is None:
            def move_a (pos):
                self.area_view.area.a = pos
            def move_b (pos):
                self.area_view.area.b = pos
            objs = [ [ self.area_view.area.a, 0.2, move_a ],
                    [ self.area_view.area.b, 0.2, move_b ] ]
            for o in self.area_view.area.table.obstacles:
                objs.append ([ o.pos, o.radius, o ])
            for obj in objs:
                opos = vector (obj[0])
                radius = obj[1]
                if abs (opos - pos) < radius:
                    self.move = obj[2]
                    break
            if self.move is not None:
                move (self.move, None)
        else:
            move (self.move, pos)
            self.move = None
        self.update ()

    def new_round_obstacle (self):
        o = RoundObstacle (self.new_obstacle_radius.get ())
        o.pos = (5, -5)
        self.area_view.area.table.obstacles.append (o)
        self.update ()

if __name__ == '__main__':
    app = TestTable ()
    o = RoundObstacle (2)
    o.pos = (5, 5)
    app.area_view.area.table.obstacles.append (o)
    app.update (False)
    app.mainloop ()
