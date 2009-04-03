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
from math import pi
import time

from inter import Inter, Obstacle
from dist_sensor import DistSensor
from path import Path
from Tkinter import *
from mex.node import Node
from mex.msg import Msg

import asserv
import io

class InterNode (Inter):
    """Inter, coupled with a mex Node."""

    # There is 900 tick per seconds, to permit AVR to have a 4.44444 ms
    # period.
    TICK = 900.0

    def __init__ (self):
        Inter.__init__ (self)
        self.node = Node ()
        self.asserv_link = asserv.Mex (self.node)
        self.asserv_link.position.register (self.notify_position)
        self.asserv_link.aux[0].register (self.notify_aux0)
        self.io_link = io.Mex (self.node)
        self.notify_jack ()
        self.notify_color_switch ()
        self.jackVar.trace_variable ('w',
                lambda *args: self.notify_jack ())
        self.colorVar.trace_variable ('w',
                lambda *args: self.notify_color_switch ())
        for i in range (len (self.io_link.servo)):
            self.io_link.servo[i].register (
                    lambda i = i: self.notify_servo (i))
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
            s.register (self.update_sharps)
        self.update_sharps ()
        self.path = Path (self.tableview.table)
        self.io_link.path.register (self.notify_path)

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

    def notify_position (self):
        self.tableview.robot.pos = self.asserv_link.position.pos
        self.tableview.robot.angle = self.asserv_link.position.angle
        self.tableview.robot.update ()

    def notify_aux0 (self):
        self.actuatorview.arm.angle = self.asserv_link.aux[0].angle
        self.actuatorview.arm.update ()

    def notify_jack (self):
        self.io_link.jack.state = self.jackVar.get ()
        self.io_link.jack.notify ()

    def notify_color_switch (self):
        self.io_link.color_switch.state = self.colorVar.get ()
        self.io_link.color_switch.notify ()

    def notify_servo (self, i):
        servo = self.io_link.servo[i]
        trap = self.actuatorview.rear.traps[i]
        trap.pos = servo.value
        trap.update ()

    def update_sharps (self):
        for ds, adc in zip (self.dist_sensors, self.io_link.adc):
            d = ds.distance or 800
            d /= 10
            if d > 10:
                v = 0.000571429 * d*d + -0.0752381 * d + 2.89107
            else:
                v = 2.2 / 10 * d
            v = int (round (v * 1024 / 5))
            assert v >= 0 and v < 1024
            adc.value = v
            adc.notify ()

    def notify_path (self):
        self.path.path = self.io_link.path.path
        self.path.update ()

    def place_obstacle (self, ev):
        pos = self.tableview.screen_coord ((ev.x, ev.y))
        if self.obstacles:
            self.obstacles[0].pos = pos
        else:
            self.obstacles.append (Obstacle (self.tableview.table, pos, 150))
        for d in self.obstacles + self.dist_sensors:
            d.update ()
        self.update ()

    def show_sensors (self):
        hide = not self.showSensorsVar.get ()
        for i in self.dist_sensors:
            i.hide = hide
            i.update ()
        self.update ()

if __name__ == '__main__':
    import mex.hub
    import utils.forked
    h = mex.hub.Hub (min_clients = 1)
    fh = utils.forked.Forked (h.wait)
    try:
        app = InterNode ()
        app.mainloop()
    finally:
        fh.kill ()
        import time
        time.sleep (1)
