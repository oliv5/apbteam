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
"""Drawable and DrawableCanvas."""
from math import sqrt, degrees
import trans_matrix
import Tkinter

__all__ = ('Drawable', 'DrawableCanvas')

class Drawable:
    """Define a drawable area with embedded transformations."""

    def __init__ (self, onto):
	"""Initialise the drawable."""
	self.onto = onto
	self.trans_matrix = trans_matrix.TransMatrix ()
	self.items = [ ]
	self.trans_apply = self.trans_matrix.apply
	self.trans_apply_angle = self.trans_matrix.apply_angle
	self.trans_apply_distance = self.trans_matrix.apply_distance
	self.trans_rotate = self.trans_matrix.rotate
	self.trans_translate = self.trans_matrix.translate
	self.trans_scale = self.trans_matrix.scale

    def __draw_rectangle (self, p1, p2, **kw):
	if 'outline' not in kw:
	    kw = kw.copy ()
	    kw['outline'] = 'black'
	p = self.trans_apply (p1, (p2[0], p1[1]), p2, (p1[0], p2[1]))
	return self.onto.__draw_polygon (*p, **kw)

    def __draw_line (self, *p, **kw):
	p = self.trans_apply (*p)
	return self.onto.__draw_line (*p, **kw)

    def __draw_polygon (self, *p, **kw):
	p = self.trans_apply (*p)
	return self.onto.__draw_polygon (*p, **kw)

    def __draw_circle (self, p, r, **kw):
	p = self.trans_apply (p)
	r = self.trans_apply_distance (r)
	return self.onto.__draw_circle (p, r, **kw)

    def __draw_arc (self, p, r, **kw):
	p = self.trans_apply (p)
	r = self.trans_apply_distance (r)
	if 'start' in kw:
	    kw = kw.copy ()
	    kw['start'] = self.trans_apply_angle (kw['start'])
	    import math
	return self.onto.__draw_arc (p, r, **kw)

    def draw_rectangle (self, *p, **kw):
	"""Draw a rectangle."""
	self.items.append (self.__draw_rectangle (*p, **kw))

    def draw_line (self, *p, **kw):
	"""Draw a line."""
	self.items.append (self.__draw_line (*p, **kw))

    def draw_polygon (self, *p, **kw):
	"""Draw a line."""
	self.items.append (self.__draw_polygon (*p, **kw))

    def draw_circle (self, p, r, **kw):
	"""Draw a circle of the given radius centered on p."""
	self.items.append (self.__draw_circle (p, r, **kw))

    def draw_arc (self, p, r, **kw):
	"""Draw a arc of the given radius centered on p."""
	self.items.append (self.__draw_arc (p, r, **kw))

    def trans_reset (self):
	"""Reset transformations."""
	self.trans_matrix.identity ()

    def reset (self):
	self.__delete (*self.items)
	self.items = [ ]
	self.trans_reset ()

    def __delete (self, *list):
	"""Delete a list of items."""
	self.onto.__delete (*list)


class DrawableCanvas(Tkinter.Canvas):
    """Extend a Tkinter.Canvas to use Drawable on it.  User should implement
    the draw method."""

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

    def __resize (self, ev):
	# Compute new scale.
	w, h = float (ev.width), float (ev.height)
	self.__scale = min (w / self.__width, h / self.__height)
	self.__xoffset = w / 2 + self.__xorigin * self.__scale
	self.__yoffset = h / 2 - self.__yorigin * self.__scale
	# Redraw.
	self.draw ()

    def _Drawable__draw_line (self, *p, **kw):
	p = self.__coord (*p)
	return self.create_line (*p, **kw)

    def _Drawable__draw_polygon (self, *p, **kw):
	p = self.__coord (*p)
	return self.create_polygon (*p, **kw)

    def _Drawable__draw_circle (self, p, r, **kw):
	p, = self.__coord (p)
	r = r * self.__scale
	p1 = (p[0] - r, p[1] - r)
	p2 = (p[0] + r, p[1] + r)
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
	return self.create_arc (p1, p2, **kw)

    def _Drawable__delete (self, *list):
	self.delete (*list)

    def __coord (self, *args):
	return [ (i[0] * self.__scale + self.__xoffset,
	    -i[1] * self.__scale + self.__yoffset) for i in args ]

    def screen_coord (self, screen):
	"""Return drawable coordinates corresponding to the given screen
	coordinates."""
	return ((self.canvasx (screen[0]) - self.__xoffset) / self.__scale,
		-(self.canvasy (screen[1]) - self.__yoffset) / self.__scale)

