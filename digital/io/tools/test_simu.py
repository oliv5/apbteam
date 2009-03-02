# io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
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
# }}} */
import math

import mex.hub
import utils.forked

import asserv
import asserv.init
from io import Io
import io.init
from proto.popen_io import PopenIO

from inter.inter_node import InterNode
from Tkinter import *

class TestSimu (InterNode):
    """Inter, with simulated programs."""

    robot_start_pos = ((200, 2100 - 70, math.radians (-90)),
            (3000 - 200, 2100 - 70, math.radians (-90)))

    def __init__ (self, asserv_cmd, io_cmd):
        # Hub.
        self.hub = mex.hub.Hub (min_clients = 2)
        self.forked_hub = utils.forked.Forked (self.hub.wait)
        # InterNode.
        InterNode.__init__ (self)
        def time ():
            return self.node.date / self.TICK
        # Asserv.
        self.asserv = asserv.Proto (PopenIO (asserv_cmd), time,
                **asserv.init.host)
        self.asserv.async = True
        self.tk.createfilehandler (self.asserv, READABLE, self.asserv_read)
        # Io.
        self.io = Io (PopenIO (io_cmd), time, **io.init.host)
        self.io.async = True
        self.tk.createfilehandler (self.io, READABLE, self.io_read)
        # Color switch.
        self.change_color ()
        self.colorVar.trace_variable ('w', self.change_color)

    def close (self):
        self.forked_hub.kill ()
        import time
        time.sleep (1)
        self.asserv.close ()
        self.io.close ()

    def asserv_read (self, file, mask):
        self.asserv.proto.read ()
        self.asserv.proto.sync ()

    def io_read (self, file, mask):
        self.io.proto.read ()
        self.io.proto.sync ()

    def step (self):
        """Overide step to handle retransmissions, could be made cleaner using
        simulated time."""
        InterNode.step (self)
        self.asserv.proto.sync ()
        self.io.proto.sync ()

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
