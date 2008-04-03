from math import sqrt, degrees
import trans_matrix
import Tkinter

__all__ = ('Drawable', 'DrawableCanvas')

class Drawable:
    """Define a drawable area with embeded transformations."""

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
    """Extend a Tkinter.Canvas to use Drawable on it."""

    def _Drawable__draw_line (self, *p, **kw):
	return self.create_line (*p, **kw)

    def _Drawable__draw_polygon (self, *p, **kw):
	return self.create_polygon (*p, **kw)

    def _Drawable__draw_circle (self, p, r, **kw):
	p1 = (p[0] - r, p[1] - r)
	p2 = (p[0] + r, p[1] + r)
	return self.create_oval (p1, p2, **kw)

    def _Drawable__draw_arc (self, p, r, **kw):
	p1 = (p[0] - r, p[1] - r)
	p2 = (p[0] + r, p[1] + r)
	for k in ('start', 'extent'):
	    if k in kw:
		kw = kw.copy ()
		# Tk is working upside down.
		kw[k] = -degrees (kw[k])
	return self.create_arc (p1, p2, **kw)

    def _Drawable__delete (self, *list):
	self.delete (*list)
