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
"""Inter, coupled with a mex Node."""
if __name__ == '__main__':
    import sys
    sys.path.append (sys.path[0] + '/../mex')

from inter import Inter
from Tkinter import *
from mex.node import Node
from mex.msg import Msg

class InterNode (Inter):
    """Inter, coupled with a mex Node."""

    # There is 900 tick per seconds, to permit AVR to have a 4.44444 ms
    # period.
    TICK = 900.0

    IO_JACK = 0xb0
    IO_COLOR = 0xb1
    IO_SERVO = 0xb2

    def __init__ (self):
	Inter.__init__ (self)
	self.node = Node ()
	self.node.register (0xa0, self.handle_asserv_0)
	self.node.register (0xa8, self.handle_asserv_8)
	self.node.register (self.IO_JACK, self.handle_IO_JACK)
	self.node.register (self.IO_COLOR, self.handle_IO_COLOR)
	self.node.register (self.IO_SERVO, self.handle_IO_SERVO)
	self.tk.createfilehandler (self.node, READABLE, self.read)
	self.step_after = None

    def createWidgets (self):
	Inter.createWidgets (self)
	self.nowLabel = Label (self.rightFrame, text = 'Now: 0 s')
	self.nowLabel.pack ()
	self.stepButton = Button (self.rightFrame, text = 'Step',
		command = self.step)
	self.stepButton.pack ()
	self.stepSizeScale = Scale (self.rightFrame, orient = HORIZONTAL,
		from_ = 0.1, to = 1.0, resolution = 0.1)
	self.stepSizeScale.pack ()
	self.playVar = IntVar ()
	self.playButton = Checkbutton (self.rightFrame, variable =
		self.playVar, text = 'Play', command = self.play)
	self.playButton.pack ()

    def step (self):
	"""Do a step.  Signal to the Hub we are ready to wait to the next step
	date."""
	self.node.wait_async (self.node.date
		+ int (self.stepSizeScale.get () * self.TICK))
	self.step_after = None

    def play (self):
	"""Activate auto-steping."""
	if self.playVar.get ():
	    if self.step_after is None:
		self.step ()
	    self.stepButton.configure (state = DISABLED)
	else:
	    if self.step_after is not None:
		self.after_cancel (self.step_after)
		self.step_after = None
	    self.stepButton.configure (state = NORMAL)

    def read (self, file, mask):
	"""Handle event on the Node."""
	self.node.read ()
	if self.node.sync ():
	    self.synced = True
	    self.nowLabel.configure (text = 'Now: %.1f s' % (self.node.date /
		self.TICK))
	    self.update ()
	    if self.playVar.get ():
		self.step_after = self.after (int (self.stepSizeScale.get ()
		    * self.TICK), self.step)

    def handle_asserv_0 (self, msg):
	x, y, a = msg.pop ('hhl')
	self.tableview.robot.pos = (x, y)
	self.tableview.robot.angle = float (a) / 1024
	self.update (self.tableview.robot)

    def handle_asserv_8 (self, msg):
	a, = msg.pop ('l')
	self.actuatorview.arm.angle = float (a) / 1024
	self.update (self.actuatorview.arm)

    def handle_IO_JACK (self, msg):
	m = Msg (self.IO_JACK)
	m.push ('B', self.jackVar.get ())
	self.node.response (m)

    def handle_IO_COLOR (self, msg):
	m = Msg (self.IO_COLOR)
	m.push ('B', self.colorVar.get ())
	self.node.response (m)

    def handle_IO_SERVO (self, msg):
	for t in self.actuatorview.rear.traps:
	    t.pos = float (msg.pop ('B')[0]) / 255
	self.update (self.actuatorview.rear)

if __name__ == '__main__':
    import mex.hub
    import mex.forked
    h = mex.hub.Hub (min_clients = 1)
    fh = mex.forked.Forked (h.wait)
    try:
	app = InterNode ()
	app.mainloop()
    finally:
	fh.kill ()
	import time
	time.sleep (1)
