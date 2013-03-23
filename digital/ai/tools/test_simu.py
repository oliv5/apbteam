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

    def __init__ (self, robot_class, robot_nb = 1,
            color_switch_set_pos = False):
        # Hub.
        self.hub = mex.hub.Hub (min_clients = 1 + robot_class.client_nb
                * robot_nb)
        self.forked_hub = utils.forked.Forked (self.hub.wait)
        # InterNode.
        InterNode.__init__ (self, robot_class.tick)
        def proto_time ():
            return self.node.date / self.node.tick
        # Robot instances.
        self.robots = [ robot_class (proto_time, 'robot%d' % i)
                for i in xrange (robot_nb) ]
        for r in self.robots:
            for prog in r.protos:
                prog.async = True
                def prog_read (f, mask, prog = prog):
                    try:
                        prog.proto.read ()
                        prog.proto.sync ()
                    except EOFError:
                        print "Connection closed"
                        self.tk.deletefilehandler (prog)
                        self.play_var.set (0)
                        self.play ()
                self.tk.createfilehandler (prog, READABLE, prog_read)
        # Add table.
        self.table_model = robot_class.table_model.Table ()
        self.table = robot_class.table_view.Table (self.table_view,
                self.table_model)
        self.obstacle = obstacle_model.RoundObstacle (150, 4)
        self.table_model.obstacles.append (self.obstacle)
        self.obstacle_beacon = obstacle_model.RoundObstacle (40, 5)
        self.table_model.obstacles.append (self.obstacle_beacon)
        self.obstacle_view = ObstacleWithBeacon (self.table, self.obstacle,
                self.obstacle_beacon)
        self.table_view.bind ('<2>', self.place_obstacle)
        # Add robots.
        for r in self.robots:
            r.link = r.robot_link.Bag (self.node, r.instance)
            r.model = r.robot_model.Bag (self.node, self.table_model, r.link)
            r.view = r.robot_view.Bag (self.table, self.actuator_view,
                    self.sensor_frame, r.model)
            # Color switch.
            def change_color (r = r):
                i = r.model.color_switch.state
                r.asserv.set_simu_pos (*r.robot_start_pos[i])
                if color_switch_set_pos:
                    r.asserv.set_pos (*r.robot_start_pos[i])
            r.model.color_switch.register (change_color)
            # Beacon system.
            if hasattr (r.link, 'beacon'):
                r.link.beacon.position[0].register_to (self.obstacle)

    def close (self):
        self.forked_hub.kill ()
        import time
        time.sleep (1)

    def step (self):
        """Overide step to handle retransmissions, could be made cleaner using
        simulated time."""
        InterNode.step (self)
        for r in self.robots:
            for prog in r.protos:
                prog.proto.sync ()

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
    parser.add_option ('-n', '--robot-nb', help = "number of robots",
            type = 'int', metavar = 'NB', default = 1)
    (options, args) = parser.parse_args ()
    if args:
        parser.error ("too many arguments")
    if options.robot == 'marcel':
        import marcel
        robot = marcel.Robot
    elif options.robot == 'robospierre':
        import robospierre
        robot = robospierre.Robot
    elif options.robot == 'guybrush':
        import guybrush
        robot = guybrush.Robot
    elif options.robot == 'apbirthday':
        import apbirthday
        robot = apbirthday.Robot
    else:
        parser.error ("unknown robot")
    app = test_class (robot, options.robot_nb)
    try:
        app.mainloop ()
    finally:
        app.close ()

if __name__ == '__main__':
    run ('marcel')
