# avr.path - Path finding module. {{{
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
"""Graphic interface for test_path."""
import re

from Tkinter import *
from simu.inter.drawable import *
from subprocess import Popen, PIPE

class Obstacle:
    def __init__ (self, pos, radius):
        self.pos = pos
        self.radius = radius

    def move (self, pos):
        self.pos = pos

class Area (Drawable):

    def __init__ (self, onto, border_min, border_max):
        Drawable.__init__ (self, onto)
        self.border_min = border_min
        self.border_max = border_max
        self.border = None
        self.src = None
        self.dst = None
        self.obstacles = [ ]
        self.path = [ ]
        self.points = { }
        self.arcs = [ ]
        self.draw_arcs = True

    def draw (self):
        self.reset ()
        self.draw_rectangle (self.border_min, self.border_max, fill = 'white')
        for o in self.obstacles:
            if o.pos is not None:
                self.draw_circle (o.pos, o.radius, fill = 'gray25')
        if self.src is not None:
            self.draw_circle (self.src, 10, fill = 'green')
        if self.dst is not None:
            self.draw_circle (self.dst, 10, fill = 'red')
        if self.draw_arcs and self.arcs:
            fmt = dict (fill = 'gray75')
            for a in self.arcs:
                self.draw_line (self.points[a[0]], self.points[a[1]], **fmt)
        if len (self.path) > 1:
            fmt = dict (fill = 'blue', arrow = LAST)
            self.draw_line (*self.path, **fmt)
    
    def test (self):
        self.src = (300, 750)
        self.dst = (1200, 750)
        self.obstacles.append (Obstacle ((600, 680), 100))
        self.obstacles.append (Obstacle ((900, 820), 100))

    def update (self):
        args = [ [ self.border_min[0], self.border_min[1], self.border_max[0],
            self.border_max[1] ], self.src, self.dst ]
        for o in self.obstacles:
            args.append ([o.pos[0], o.pos[1], o.radius])
        args = [ ','.join (str (ai) for ai in a) for a in args ]
        args[0:0] = [ './test_path.host' ]
        p = Popen (args, stdout = PIPE)
        output = p.communicate ()[0]
        del p
        output = output.split ('\n')
        r = re.compile ('^// (-?\d+), (-?\d+)$')
        r_point = re.compile ('^ (\d+) .* pos = "(-?\d+),(-?\d+)"')
        r_arc = re.compile ('^  (-?\d+) -- (-?\d+) .* label = "(\d+)"')
        self.path = [ ]
        self.points = { }
        self.arcs = [ ]
        for line in output:
            m = r.match (line)
            if m is not None:
                self.path.append (tuple (int (s) for s in m.groups ()))
            m = r_point.match (line)
            if m is not None:
                self.points[int (m.group (1))] = tuple (int (s)
                        for s in m.group (2, 3))
            m = r_arc.match (line)
            if m is not None:
                self.arcs.append (tuple (int (s) for s in m.groups ()))

class AreaView (DrawableCanvas):

    def __init__ (self, border_min, border_max, master = None):
        self.border_min = border_min
        self.border_max = border_max
        width = border_max[0] - border_min[0]
        height = border_max[1] - border_min[0]
        DrawableCanvas.__init__ (self, width * 1.1, height * 1.1, -width / 2,
                -height / 2,
                master, borderwidth = 1, relief = 'sunken',
                background = 'white')
        self.area = Area (self, border_min, border_max)
        self.area.test ()
        self.area.update ()

    def draw (self):
        self.area.draw ()

class TestPath (Frame):

    def __init__ (self, border_min, border_max, master = None):
        Frame.__init__ (self, master)
        self.pack (expand = 1, fill = 'both')
        self.createWidgets (border_min, border_max)
        self.move = None

    def createWidgets (self, border_min, border_max):
        self.rightFrame = Frame (self)
        self.rightFrame.pack (side = 'right', fill = 'y')
        self.quitButton = Button (self.rightFrame, text = 'Quit', command = self.quit)
        self.quitButton.pack (side = 'top', fill = 'x')
        self.arcsVar = IntVar ()
        self.arcsVar.set (1)
        self.arcsButton = Checkbutton (self.rightFrame,
                variable = self.arcsVar, command = self.arcs_toggle,
                text = 'Arcs', indicatoron = True)
        self.arcsButton.pack (side = 'top')
        self.areaview = AreaView (border_min, border_max, self)
        self.areaview.pack (expand = True, fill = 'both')
        self.areaview.bind ('<1>', self.click)

    def clear (self):
        self.areaview.area.path = [ ]
        self.areaview.area.points = { }
        self.areaview.area.arcs = [ ]
        self.areaview.area.draw ()

    def update (self):
        self.areaview.area.update ()
        self.areaview.area.draw ()

    def click (self, ev):
        pos = self.areaview.screen_coord ((ev.x, ev.y))
        pos = tuple (int (i) for i in pos)
        if self.move is None:
            def move_src (pos):
                self.areaview.area.src = pos
            def move_dst (pos):
                self.areaview.area.dst = pos
            objs = [ [ self.areaview.area.src, 10, move_src ],
                    [ self.areaview.area.dst, 10, move_dst ] ]
            for o in self.areaview.area.obstacles:
                objs.append ([ o.pos, o.radius, o.move ])
            for obj in objs:
                dx = obj[0][0] - pos[0]
                dy = obj[0][1] - pos[1]
                if dx * dx + dy * dy < obj[1] * obj[1]:
                    self.move = obj[2]
                    break
            if self.move is not None:
                self.move (None)
                self.clear ()
        else:
            self.move (pos)
            self.update ()
            self.move = None

    def arcs_toggle (self):
        self.areaview.area.draw_arcs = self.arcsVar.get () != 0
        print self.areaview.area.draw_arcs
        self.areaview.area.draw ()

if __name__ == '__main__':
    app = TestPath ((0, 0), (1500, 1500))
    app.mainloop ()
