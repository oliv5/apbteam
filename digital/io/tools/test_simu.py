import sys
sys.path.append (sys.path[0] + '/../../../host')
sys.path.append (sys.path[0] + '/../../asserv/tools')

import mex.hub
import utils.forked

from asserv import Asserv
import asserv.init
from proto.popen_io import PopenIO

from inter.inter_node import InterNode
from Tkinter import *

class TestSimu (InterNode):
    """Inter, with simulated programs."""

    robot_start_pos = ((200, 2100 - 70, -90),
	    (3000 - 200, 2100 - 70, -90))

    def __init__ (self, asserv_cmd, io_cmd):
	# Hub.
	self.hub = mex.hub.Hub (min_clients = 2)
	self.forked_hub = utils.forked.Forked (self.hub.wait)
	# InterNode.
	InterNode.__init__ (self)
	def time ():
	    return self.node.date / self.TICK
	# Asserv.
	self.asserv = Asserv (PopenIO (asserv_cmd), time, **asserv.init.host)
	self.asserv.async = True
	self.tk.createfilehandler (self.asserv, READABLE, self.asserv_read)
	# Color switch.
	self.change_color ()
	self.colorVar.trace_variable ('w', self.change_color)

    def close (self):
	self.forked_hub.kill ()
	import time
	time.sleep (1)
	app.asserv.close ()

    def asserv_read (self, file, mask):
	self.asserv.proto.read ()
	self.asserv.proto.sync ()

    def step (self):
	"""Overide step to handle retransmissions, could be made cleaner using
	simulated time."""
	InterNode.step (self)
	self.asserv.proto.sync ()

    def change_color (self, *dummy):
	i = self.colorVar.get ()
	self.asserv.set_simu_pos (*self.robot_start_pos[i]);

if __name__ == '__main__':
    app = TestSimu (('../../asserv/src/asserv/asserv.host', '-m', 'giboulee'),
	    ('../src/io.host'))
    try:
	app.mainloop ()
    finally:
	app.close ()
