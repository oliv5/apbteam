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
import mex.hub
import utils.forked

import simu.model.round_obstacle as obstacle_model
import simu.view.round_obstacle as obstacle_view

from simu.inter.inter_node import InterNode
from Tkinter import *

class ObstacleWithBeacon (obstacle_view.RoundObstacle):

    def __init__ (self, onto, model, beacon_model):
        obstacle_view.RoundObstacle.__init__ (self, onto, model)
        self.beacon_model = beacon_model

    def __notified (self):
        self.pos = self.model.pos
        self.update ()

    def draw (self):
        obstacle_view.RoundObstacle.draw (self)
        if self.pos:
            self.draw_circle ((0, 0), self.beacon_model.radius,
                    fill = '#505050')

class TestSimu (InterNode):
    """Interface, with simulated programs."""

    def __init__ (self, robot_class):
        # Hub.
        self.hub = mex.hub.Hub (min_clients = 4)
        self.forked_hub = utils.forked.Forked (self.hub.wait)
        # InterNode.
        InterNode.__init__ (self)
        def proto_time ():
            return self.node.date / self.node.tick
        # Robot parameters.
        robot = robot_class (proto_time)
        self.robot = robot
        # Asserv.
        self.asserv = robot.asserv
        self.asserv.async = True
        self.tk.createfilehandler (self.asserv, READABLE, self.asserv_read)
        # Mimot.
        self.mimot = robot.mimot
        self.mimot.async = True
        self.tk.createfilehandler (self.mimot, READABLE, self.mimot_read)
        # Io.
        self.io = robot.io
        self.io.async = True
        self.tk.createfilehandler (self.io, READABLE, self.io_read)
        # Add table.
        self.table_model = robot.table_model.Table ()
        self.table = robot.table_view.Table (self.table_view, self.table_model)
        self.obstacle = obstacle_model.RoundObstacle (150)
        self.table_model.obstacles.append (self.obstacle)
        self.obstacle_beacon = obstacle_model.RoundObstacle (40, 2)
        self.table_model.obstacles.append (self.obstacle_beacon)
        self.obstacle_view = ObstacleWithBeacon (self.table, self.obstacle,
                self.obstacle_beacon)
        self.table_view.bind ('<2>', self.place_obstacle)
        # Add robot.
        self.robot_link = robot.robot_link.Bag (self.node)
        self.robot_model = robot.robot_model.Bag (self.node, self.table_model,
                self.robot_link)
        self.robot_view = robot.robot_view.Bag (self.table,
                self.actuator_view, self.sensor_frame, self.robot_model)
        # Color switch.
        self.robot_model.color_switch.register (self.change_color)

    def close (self):
        self.forked_hub.kill ()
        import time
        time.sleep (1)

    def asserv_read (self, file, mask):
        self.asserv.proto.read ()
        self.asserv.proto.sync ()

    def mimot_read (self, file, mask):
        self.mimot.proto.read ()
        self.mimot.proto.sync ()

    def io_read (self, file, mask):
        self.io.proto.read ()
        self.io.proto.sync ()

    def step (self):
        """Overide step to handle retransmissions, could be made cleaner using
        simulated time."""
        InterNode.step (self)
        self.asserv.proto.sync ()
        self.mimot.proto.sync ()
        self.io.proto.sync ()

    def change_color (self, *dummy):
        i = self.robot_model.color_switch.state
        self.asserv.set_simu_pos (*self.robot.robot_start_pos[i]);

    def place_obstacle (self, ev):
        pos = self.table_view.screen_coord ((ev.x, ev.y))
        self.obstacle.pos = pos
        self.obstacle_beacon.pos = pos
        self.obstacle.notify ()
        self.obstacle_beacon.notify ()

def run (default_robot, test_class = TestSimu):
    import optparse
    parser = optparse.OptionParser ()
    parser.add_option ('-r', '--robot', help = "use specified robot",
            metavar = 'NAME', default = default_robot)
    (options, args) = parser.parse_args ()
    if args:
        parser.error ("too many arguments")
    if options.robot == 'marcel':
        import marcel
        robot = marcel.Robot
    else:
        parser.error ("unknown robot")
    app = test_class (robot)
    app.mainloop ()
    app.close ()

if __name__ == '__main__':
    run ('marcel')
