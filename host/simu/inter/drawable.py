# simu - Robot simulation. {{{
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
"""Drawable and DrawableCanvas."""
from math import sqrt, degrees, pi, cos, sin
import simu.utils.trans_matrix
import Tkinter

__all__ = ('Drawable', 'DrawableCanvas')

class Drawable:
    """Define a drawable area with embedded transformations."""

    def __init__ (self, onto):
        """Initialise the drawable."""
        self.__onto = onto
        self.__trans_matrix = simu.utils.trans_matrix.TransMatrix ()
        self.__items = [ ]
        self.trans_apply = self.__trans_matrix.apply
        self.trans_apply_angle = self.__trans_matrix.apply_angle
        self.trans_apply_distance = self.__trans_matrix.apply_distance
        self.trans_rotate = self.__trans_matrix.rotate
        self.trans_translate = self.__trans_matrix.translate
        self.trans_scale = self.__trans_matrix.scale
        self.trans_identity = self.__trans_matrix.identity
        self.trans_push = self.__trans_matrix.push
        self.trans_pop = self.__trans_matrix.pop
        self.__children = [ ]
        self.children = self.__children
        self.__onto.__children.append (self)

    def __trans_apply_kw (self, kw):
        if 'width' in kw:
            kw = kw.copy ()
            kw['width'] = self.trans_apply_distance (kw['width'])
        return kw

    def __draw_rectangle (self, p1, p2, **kw):
        if 'outline' not in kw:
            kw = kw.copy ()
            kw['outline'] = 'black'
        kw = self.__trans_apply_kw (kw)
        p = self.trans_apply (p1, (p2[0], p1[1]), p2, (p1[0], p2[1]))
        return self.__onto.__draw_polygon (*p, **kw)

    def __draw_line (self, *p, **kw):
        p = self.trans_apply (*p)
        kw = self.__trans_apply_kw (kw)
        return self.__onto.__draw_line (*p, **kw)

    def __draw_polygon (self, *p, **kw):
        p = self.trans_apply (*p)
        kw = self.__trans_apply_kw (kw)
        return self.__onto.__draw_polygon (*p, **kw)

    def __draw_circle (self, p, r, **kw):
        p = self.trans_apply (p)
        r = self.trans_apply_distance (r)
        kw = self.__trans_apply_kw (kw)
        return self.__onto.__draw_circle (p, r, **kw)

    def __draw_oval (self, p, rx, ry, **kw):
        v = [ ]
        n = 8
        sx = rx / cos (pi / n)
        sy = ry / cos (pi / n)
        for i in xrange (0, n):
            a = i * 2 * pi / n
            v.append ((p[0] + cos (a) * sx, p[1] + sin (a) * sy))
        kw = kw.copy ()
        if 'fill' not in kw:
            kw['fill'] = ''
        if 'outline' not in kw:
            kw['outline'] = 'black'
        kw['smooth'] = True
        kw = self.__trans_apply_kw (kw)
        return self.__draw_polygon (*v, **kw)

    def __draw_arc (self, p, r, **kw):
        p = self.trans_apply (p)
        r = self.trans_apply_distance (r)
        if 'start' in kw:
            kw = kw.copy ()
            kw['start'] = self.trans_apply_angle (kw['start'])
        kw = self.__trans_apply_kw (kw)
        return self.__onto.__draw_arc (p, r, **kw)

    def __draw_text (self, p, **kw):
        p = self.trans_apply (p)
        return self.__onto.__draw_text (p, **kw)

    def draw_rectangle (self, *p, **kw):
        """Draw a rectangle."""
        self.__items.append (self.__draw_rectangle (*p, **kw))

    def draw_line (self, *p, **kw):
        """Draw a line."""
        self.__items.append (self.__draw_line (*p, **kw))

    def draw_polygon (self, *p, **kw):
        """Draw a line."""
        self.__items.append (self.__draw_polygon (*p, **kw))

    def draw_circle (self, p, r, **kw):
        """Draw a circle of the given radius centered on p."""
        self.__items.append (self.__draw_circle (p, r, **kw))

    def draw_oval (self, p, rx, ry, **kw):
        """Draw an oval of the given radii (rx along x, ry along y), centered
        on p."""
        self.__items.append (self.__draw_oval (p, rx, ry, **kw))

    def draw_arc (self, p, r, **kw):
        """Draw a arc of the given radius centered on p."""
        self.__items.append (self.__draw_arc (p, r, **kw))

    def draw_text (self, p, **kw):
        """Draw text at given position."""
        self.__items.append (self.__draw_text (p, **kw))

    def reset (self):
        """Clear all drawn items, reset transformations."""
        for i in self.__children:
            i.reset ()
        self.__delete (*self.__items)
        self.__items = [ ]
        self.trans_identity ()

    def __delete (self, *list):
        """Delete a list of items."""
        self.__onto.__delete (*list)

    def update (self, *list):
        """Add provided arguments to update list, or self if no argument
        provided."""
        if list:
            self.__onto.update (*list)
        else:
            self.__onto.update (self)

    def draw (self):
        """Default drawing method which redraw every children."""
        if self.__children:
            self.update (*self.__children)


class DrawableCanvas(Tkinter.Canvas):
    """Extend a Tkinter.Canvas to use Drawable on it.  Children are drawn on
    redraw."""

    def __init__ (self, width, height, xorigin, yorigin, master = None, **kw):
        """Initialise a DrawableCanvas.  The width and height parameters
        define the requested drawable area virtual size.  The xorigin and
        yorigin parameters define origin of the virtual coordinates relative
        to the drawable center."""
        Tkinter.Canvas.__init__ (self, master, **kw)
        self.__width = width
        self.__height = height
        self.__xorigin = xorigin
        self.__yorigin = yorigin
        self.bind ('<Configure>', self.__resize)
        self.__updated = [ ]
        self._Drawable__children = [ ]

    def resize (self, width, height, xorigin = None, yorigin = None):
        """Change size given at initialisation.  Will be used on next
        configure event."""
        self.__width = width
        self.__height = height
        if xorigin is not None:
            self.__xorigin = xorigin
        if yorigin is not None:
            self.__yorigin = yorigin

    def __resize (self, ev):
        # Compute new scale.
        w, h = float (ev.width), float (ev.height)
        self.__scale = min (w / self.__width, h / self.__height)
        self.__xoffset = w / 2 + self.__xorigin * self.__scale
        self.__yoffset = h / 2 - self.__yorigin * self.__scale
        # Redraw.
        self.draw ()

    def __trans_apply_kw (self, kw):
        if 'width' in kw:
            kw = kw.copy ()
            kw['width'] *= self.__scale
        return kw

    def _Drawable__draw_line (self, *p, **kw):
        p = self.__coord (*p)
        kw = self.__trans_apply_kw (kw)
        return self.create_line (*p, **kw)

    def _Drawable__draw_polygon (self, *p, **kw):
        p = self.__coord (*p)
        kw = self.__trans_apply_kw (kw)
        return self.create_polygon (*p, **kw)

    def _Drawable__draw_circle (self, p, r, **kw):
        p, = self.__coord (p)
        r = r * self.__scale
        p1 = (p[0] - r, p[1] - r)
        p2 = (p[0] + r, p[1] + r)
        kw = self.__trans_apply_kw (kw)
        return self.create_oval (p1, p2, **kw)

    def _Drawable__draw_arc (self, p, r, **kw):
        p, = self.__coord (p)
        r = r * self.__scale
        p1 = (p[0] - r, p[1] - r)
        p2 = (p[0] + r, p[1] + r)
        for k in ('start', 'extent'):
            if k in kw:
                kw = kw.copy ()
                kw[k] = degrees (kw[k])
        kw = self.__trans_apply_kw (kw)
        return self.create_arc (p1, p2, **kw)

    def _Drawable__draw_text (self, p, **kw):
        p, = self.__coord (p)
        return self.create_text (p, **kw)

    def _Drawable__delete (self, *list):
        self.delete (*list)

    def update (self, *list):
        """If called with arguments, add them to the update list.
        Else, redraw all element in the update list."""
        if list:
            for i in list:
                if i not in self.__updated:
                    self.__updated.append (i)
        else:
            while self.__updated:
                updated = self.__updated
                self.__updated = [ ]
                for i in updated:
                    i.draw ()

    def draw (self):
        """Default drawing method which redraw every children."""
        if self._Drawable__children:
            self.update (*self._Drawable__children)
        self.update ()

    def __coord (self, *args):
        return [ (i[0] * self.__scale + self.__xoffset,
            -i[1] * self.__scale + self.__yoffset) for i in args ]

    def screen_coord (self, screen):
        """Return drawable coordinates corresponding to the given screen
        coordinates."""
        return ((self.canvasx (screen[0]) - self.__xoffset) / self.__scale,
                -(self.canvasy (screen[1]) - self.__yoffset) / self.__scale)

