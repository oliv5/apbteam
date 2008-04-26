# inter_asserv - Asserv interface. {{{
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
"""Inter, communicating with the asserv board."""
import sys
sys.path.append (sys.path[0] + '/../../../host/proto')
sys.path.append (sys.path[0] + '/../../../host/inter')

from asserv import Asserv
import init
import popen_io
import serial

import math

from inter import Inter
from Tkinter import *


class InterAsserv (Inter):
    """Inter, communicating with the asserv board."""

    def __init__ (self, argv):
	# Asserv.
	if argv[0] == '!':
	    io = popen_io.PopenIO (argv[1:])
	    i = init.host
	else:
	    io = serial.Serial (argv[0])
	    i = init.target
	self.a = Asserv (io, **i)
	self.a.async = True
	self.a.register_pos (self.pos)
	# Inter.
	Inter.__init__ (self)
	self.tk.createfilehandler (self.a, READABLE, self.read)

    def createWidgets (self):
	Inter.createWidgets (self)
	self.ArggButton = Button (self.rightFrame, text = 'Argggg!',
		command = self.emergency_stop)
	self.ArggButton.pack ()
	self.actionVar = StringVar ()
	self.actionVar.set ('goto')
	self.actionSetPosButton = Radiobutton (self.rightFrame,
		text = 'set pos', value = 'set_pos',
		variable = self.actionVar)
	self.actionSetPosButton.pack ()
	self.actionGotoButton = Radiobutton (self.rightFrame,
		text = 'goto', value = 'goto',
		variable = self.actionVar)
	self.actionGotoButton.pack ()
	self.backwardOkVar = IntVar ()
	self.backwardOkButton = Checkbutton (self.rightFrame,
		text = 'backward ok', variable = self.backwardOkVar)
	self.backwardOkButton.pack ()

	self.tableview.configure (cursor = 'crosshair')
	self.tableview.bind ('<1>', self.button1)
	self.tableview.bind ('<3>', self.button3)

    def read (self, file, mask):
	"""Handle asserv events."""
	self.a.proto.read ()
	self.a.proto.sync ()

    def pos (self, x, y, a):
	self.tableview.robot.pos = (x, y)
	self.tableview.robot.angle = float (a) / 180 * math.pi
	self.update (self.tableview.robot)
	self.update ()

    def button1 (self, ev):
	x, y = self.tableview.screen_coord ((ev.x, ev.y))
	action = self.actionVar.get ()
	if action == 'set_pos':
	    self.a.set_pos (x, y)
	elif action == 'goto':
	    self.a.goto (x, y, self.backwardOkVar.get ())
	else:
	    assert 0

    def button3 (self, ev):
	x, y = self.tableview.screen_coord ((ev.x, ev.y))
	a = math.degrees (math.atan2 (y - self.tableview.robot.pos[1],
	    x - self.tableview.robot.pos[0]))
	action = self.actionVar.get ()
	if action == 'set_pos':
	    self.a.set_pos (a = a)
	elif action == 'goto':
	    self.a.goto_angle (a)

    def emergency_stop (self):
	self.a.free ()

if __name__ == '__main__':
    app = InterAsserv (sys.argv[1:])
    try:
	app.mainloop ()
    finally:
	app.a.close ()
