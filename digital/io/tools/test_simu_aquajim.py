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
# }}}
import math

import mex.hub
import utils.forked

import asserv
import asserv.init
import io
import io.init
from proto.popen_io import PopenIO

import simu.model.table_eurobot2009 as table_model
import simu.view.table_eurobot2009 as table

import simu.model.round_obstacle as obstacle_model
import simu.view.round_obstacle as obstacle_view

import simu.robots.aquajim.link.bag as robot_link
import simu.robots.aquajim.model.bag as robot_model
import simu.robots.aquajim.view.bag as robot_view

from simu.inter.inter_node import InterNode
from Tkinter import *

class TestSimu (InterNode):
    """Interface, with simulated programs."""

    robot_start_pos = {
            False: (300, 2100 - 305, math.radians (-270)),
            True: (3000 - 300, 2100 - 305, math.radians (-270))
            }

    def __init__ (self, asserv_cmd, io_cmd):
        # Hub.
        self.hub = mex.hub.Hub (min_clients = 2)
        self.forked_hub = utils.forked.Forked (self.hub.wait)
        # InterNode.
        InterNode.__init__ (self)
        def time ():
            return self.node.date / self.node.tick
        # Asserv.
        self.asserv = asserv.Proto (PopenIO (asserv_cmd), time,
                **asserv.init.host)
        self.asserv.async = True
        self.tk.createfilehandler (self.asserv, READABLE, self.asserv_read)
        # Io.
        self.io = io.Proto (PopenIO (io_cmd), time, **io.init.host)
        self.io.async = True
        self.tk.createfilehandler (self.io, READABLE, self.io_read)
        # Add table.
        self.table_model = table_model.Table ()
        self.table = table.Table (self.table_view, self.table_model)
        self.obstacle = obstacle_model.RoundObstacle (150)
        self.table_model.obstacles.append (self.obstacle)
        self.obstacle_view = obstacle_view.RoundObstacle (self.table,
                self.obstacle)
        self.table_view.bind ('<2>', self.place_obstacle)
        # Add robot.
        self.robot_link = robot_link.Bag (self.node)
        self.robot_model = robot_model.Bag (self.node, self.table_model,
                self.robot_link)
        self.robot_view = robot_view.Bag (self.table, self.actuator_view,
                self.sensor_frame, self.robot_model)
        # Color switch.
        self.robot_model.color_switch.register (self.change_color)
        self.change_color ()

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
        i = self.robot_model.color_switch.state
        self.asserv.set_simu_pos (*self.robot_start_pos[i]);

    def place_obstacle (self, ev):
        pos = self.table_view.screen_coord ((ev.x, ev.y))
        self.obstacle.pos = pos
        self.obstacle.notify ()

if __name__ == '__main__':
    app = TestSimu (('../../asserv/src/asserv/asserv.host', '-m', 'aquajim'),
            ('../src/io.host'))
    app.mainloop ()
