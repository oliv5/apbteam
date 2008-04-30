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

from inter import Inter, Obstacle
from dist_sensor import DistSensor
from path import Path
from Tkinter import *
from mex.node import Node
from mex.msg import Msg
from math import pi
import time

class InterNode (Inter):
    """Inter, coupled with a mex Node."""

    # There is 900 tick per seconds, to permit AVR to have a 4.44444 ms
    # period.
    TICK = 900.0

    IO_JACK = 0xb0
    IO_COLOR = 0xb1
    IO_SERVO = 0xb2
    IO_SHARPS = 0xb3
    IO_PATH = 0xb4

    def __init__ (self):
	Inter.__init__ (self)
	self.node = Node ()
	self.node.register (0xa0, self.handle_asserv_0)
	self.node.register (0xa8, self.handle_asserv_8)
	self.node.register (self.IO_JACK, self.handle_IO_JACK)
	self.node.register (self.IO_COLOR, self.handle_IO_COLOR)
	self.node.register (self.IO_SERVO, self.handle_IO_SERVO)
	self.node.register (self.IO_SHARPS, self.handle_IO_SHARPS)
	self.node.register (self.IO_PATH, self.handle_IO_PATH)
	self.tk.createfilehandler (self.node, READABLE, self.read)
	self.date = 0
	self.synced = True
	self.step_after = None
	self.step_time = None
	self.obstacles = [ ]
	self.dist_sensors = [
		DistSensor (self.tableview.robot, (150, 127), 0, 800),
		DistSensor (self.tableview.robot, (150, 0), 0, 800),
		DistSensor (self.tableview.robot, (150, -127), 0, 800),
		DistSensor (self.tableview.robot, (-70, 100), pi, 800),
		DistSensor (self.tableview.robot, (-70, -100), pi, 800),
		]
	for s in self.dist_sensors:
	    s.obstacles = self.obstacles
	    s.hide = True
	self.tableview.robot.drawn.extend (self.dist_sensors)
	self.path = Path (self.tableview.table)
	self.tableview.drawn.append (self.path)
	self.tableview

    def createWidgets (self):
	Inter.createWidgets (self)
	self.nowLabel = Label (self.rightFrame, text = 'Now: 0 s')
	self.nowLabel.pack ()
	self.stepButton = Button (self.rightFrame, text = 'Step',
		command = self.step)
	self.stepButton.pack ()
	self.stepSizeScale = Scale (self.rightFrame, orient = HORIZONTAL,
		from_ = 0.05, to = 1.0, resolution = 0.05)
	self.stepSizeScale.pack ()
	self.playVar = IntVar ()
	self.playButton = Checkbutton (self.rightFrame, variable =
		self.playVar, text = 'Play', command = self.play)
	self.playButton.pack ()
	self.tableview.bind ('<2>', self.place_obstacle)
	self.showSensorsVar = IntVar ()
	self.showSensorsButton = Checkbutton (self.sensorFrame, variable =
		self.showSensorsVar, text = 'Show sensors', command =
		self.show_sensors)
	self.showSensorsButton.pack ()

    def step (self):
	"""Do a step.  Signal to the Hub we are ready to wait to the next step
	date."""
	self.node.wait_async (self.date
		+ int (self.stepSizeScale.get () * self.TICK))
	self.synced = False
	self.step_after = None
	self.step_time = time.time ()

    def play (self):
	"""Activate auto-steping."""
	if self.playVar.get ():
	    if self.step_after is None and self.synced:
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
	if not self.synced and self.node.sync ():
	    self.synced = True
	    self.date = self.node.date
	    self.nowLabel.configure (text = 'Now: %.2f s' % (self.date
		/ self.TICK))
	    self.update ()
	    if self.playVar.get ():
		assert self.step_after is None
		next = self.step_time + self.stepSizeScale.get ()
		delay = next - time.time ()
		if delay > 0:
		    self.step_after = self.after (int (delay * 1000),
			    self.step)
		else:
		    self.step ()

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

    def handle_IO_SHARPS (self, msg):
	m = Msg (self.IO_SHARPS)
	for i in self.dist_sensors:
	    d = i.distance or 800
	    d /= 10
	    if d > 10:
		v = 0.000571429 * d*d + -0.0752381 * d + 2.89107
	    else:
		v = 2.2 / 10 * d
	    v *= 1024 / 5
	    m.push ('H', v)
	    assert v >= 0 and v < 1024
	self.node.response (m)

    def handle_IO_PATH (self, msg):
	self.path.path = [ ]
	while len (msg) > 4:
	    self.path.path.append (msg.pop ('hh'))
	self.update (self.path)

    def place_obstacle (self, ev):
	pos = self.tableview.screen_coord ((ev.x, ev.y))
	if self.obstacles:
	    self.obstacles[0].pos = pos
	else:
	    self.obstacles.append (Obstacle (self.tableview.table, pos, 150))
	    self.tableview.drawn.append (self.obstacles[0])
	self.update (*self.obstacles)
	self.update (*self.dist_sensors)
	self.update ()

    def show_sensors (self):
	hide = not self.showSensorsVar.get ()
	for i in self.dist_sensors:
	    i.hide = hide
	self.update (*self.dist_sensors)
	self.update ()

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
