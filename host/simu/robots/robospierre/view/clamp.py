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
"""Robospierre clamp."""
from simu.inter.drawable import Drawable
from simu.utils.trans_matrix import TransMatrix
from math import pi, sin, cos

from simu.view.table_eurobot2011 import YELLOW, draw_pawn

DGREY = '#404040'
GREY = '#808080'
BLACK = '#000000'

class ClampTop (Drawable):

    width = 420
    height = 420

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        self.model.register (self.__notified)

    def __notified (self):
        self.update ()

    def draw (self):
        self.reset ()
        # Draw base from top.
        self.trans_rotate (pi)
        self.draw_arc ((0, 0), 190, start = pi * 35 / 180,
                extent = pi * 110 / 180, style = 'arc',
                outline = GREY)
        self.draw_arc ((0, 0), 190, start = pi + pi * 35 / 180,
                extent = pi * 110 / 180, style = 'arc',
                outline = GREY)
        self.draw_arc ((150, 0), 110, start = pi / 2,
                extent = pi, style = 'arc', outline = GREY)
        self.draw_arc ((-150, 0), 110, start = 3 * pi / 2,
                extent = pi, style = 'arc', outline = GREY)
        self.draw_arc ((0, 150), 100, start = pi,
                extent = pi, style = 'arc', outline = GREY, dash = (2, 3))
        self.draw_line ((-40, 0), (40, 0), fill = GREY, arrow = 'last')
        # Draw slots.
        for slot in self.model.slots:
            if slot.pawn is not None:
                self.trans_push ()
                self.trans_scale (1 - slot.z / 1000.0)
                self.trans_translate ((slot.x, slot.y))
                draw_pawn (self, slot.pawn.radius, slot.pawn.kind)
                self.trans_pop ()
        # Draw clamp.
        if self.model.rotation is not None:
            self.trans_rotate (self.model.rotation)
            # Fixed side.
            self.draw_line ((0, 3), (47, 3))
            self.draw_arc ((150, 0), 103, start = pi / 2, extent = pi / 2,
                    style = 'arc')
            # Pawn.
            load = self.model.load
            if load is not None:
                self.trans_push ()
                self.trans_translate ((150, 0))
                draw_pawn (self, load.radius, load.kind)
                self.trans_pop ()
            # Mobile side.
            self.trans_rotate (-self.model.clamping / 43)
            self.draw_line ((0, -3), (47, -3))
            self.draw_arc ((150, 0), 103, start = pi, extent = pi / 2,
                    style = 'arc')

class ClampSide (Drawable):

    width = 420
    height = 370

    def __init__ (self, onto, model):
        Drawable.__init__ (self, onto)
        self.model = model
        self.model.register (self.__notified)

    def __notified (self):
        self.update ()

    def draw_pawn (self, pos, pawn):
        if pawn is not None:
            self.trans_push ()
            self.trans_translate (pos)
            self.draw_rectangle ((-100, 0), (100, 50), fill = YELLOW)
            if pawn.kind == 'king':
                self.draw_polygon ((-50, 50), (-10, 170), (-50, 170), (-50, 190),
                        (-10, 190), (-10, 230), (10, 230), (10, 190), (50, 190),
                        (50, 170), (5, 170), (50, 50), fill = YELLOW,
                        outline = BLACK)
            elif pawn.kind == 'queen':
                self.draw_polygon ((-50, 50), (-10, 180), (10, 180), (50, 50),
                        fill = YELLOW, outline = BLACK)
                self.draw_circle ((0, 180), 50, fill = YELLOW)
            self.trans_pop ()

    def draw (self):
        self.reset ()
        # Draw base from side.
        self.trans_translate ((0, -165))
        self.draw_line ((-150, 0), (150, 0), fill = GREY)
        self.draw_line ((-20, 60), (20, 60), fill = GREY)
        self.draw_line ((-20, 120), (20, 120), fill = GREY)
        self.draw_line ((0, 0), (0, 330), fill = GREY)
        # Draw slots.
        for slot in self.model.slots:
            if slot.pawn is not None:
                self.draw_pawn ((-slot.x, slot.z), slot.pawn)
        # Draw clamp.
        if self.model.rotation is not None:
            self.trans_push ()
            self.trans_translate ((0, self.model.elevation))
            m = TransMatrix ()
            m.rotate (pi + self.model.rotation)
            # 2D projection of a 3D circular clamp.
            ltip = m.apply ((150, 103))[0]
            lbase = m.apply ((47, 0))[0]
            lcenter = m.apply ((150, 0))[0]
            m.rotate (- self.model.clamping / 43)
            rtip = m.apply ((150, -103))[0]
            rbase = m.apply ((47, 0))[0]
            rcenter = m.apply ((150, 0))[0]
            s, c = sin (self.model.rotation), cos (self.model.rotation)
            dattr = dict (outline = BLACK, fill = DGREY)
            attr = dict (outline = BLACK, fill = GREY)
            if c >= 0:
                if s >= 0:
                    self.draw_rectangle ((lbase, 10), (lcenter + 103, 40), **dattr)
                    self.draw_rectangle ((rtip, 10), (rbase, 40), **dattr)
                    self.draw_pawn ((lcenter, 0), self.model.load)
                    self.draw_rectangle ((ltip, 10), (lcenter + 103, 40), **attr)
                else:
                    self.draw_rectangle ((rtip, 10), (rcenter + 103, 40), **dattr)
                    self.draw_pawn ((lcenter, 0), self.model.load)
                    self.draw_rectangle ((ltip, 10), (lbase, 40), **attr)
                    self.draw_rectangle ((rbase, 10), (rcenter + 103, 40), **attr)
            else:
                if s >= 0:
                    self.draw_rectangle ((lbase, 10), (ltip, 40), **dattr)
                    self.draw_rectangle ((rbase, 10), (rcenter - 103, 40), **dattr)
                    self.draw_pawn ((lcenter, 0), self.model.load)
                    self.draw_rectangle ((rtip, 10), (rcenter - 103, 40), **attr)
                else:
                    self.draw_rectangle ((ltip, 10), (lcenter - 103, 40), **dattr)
                    self.draw_pawn ((lcenter, 0), self.model.load)
                    self.draw_rectangle ((lbase, 10), (lcenter - 103, 40), **attr)
                    self.draw_rectangle ((rbase, 10), (rtip, 40), **attr)
            self.trans_pop ()
        # Draw doors.
        for slot in self.model.slots:
            if slot.door_motor is not None:
                self.trans_push ()
                self.trans_translate ((-slot.x, slot.z + 50))
                self.trans_rotate (-0.5 * pi + slot.door_motor.angle)
                self.draw_line ((0, 0), (40, 0))
                self.trans_pop ()

