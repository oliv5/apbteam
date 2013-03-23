# simu - Robot simulation. {{{
#
# Copyright (C) 2009 Nicolas Schodet
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
"""Simulation interface coupled with a mex node."""
from simu.inter.inter import Inter
from mex.node import Node
from Tkinter import *
import time

class InterNode (Inter):

    def __init__ (self, tick):
        Inter.__init__ (self)
        # Create node and bind to Tk.
        self.node = Node ()
        self.node.tick = tick # tick/s
        self.tk.createfilehandler (self.node, READABLE, self.read)
        # Animation attributes.
        self.date = 0
        self.synced = True
        self.step_after = None
        self.step_time = None

    def create_widgets (self):
        Inter.create_widgets (self)
        self.now_label = Label (self.right_frame, text = 'Now: 0 s')
        self.now_label.pack ()
        self.step_button = Button (self.right_frame, text = 'Step',
                command = self.step)
        self.step_button.pack ()
        self.step_size_scale = Scale (self.right_frame, orient = HORIZONTAL,
                from_ = 0.05, to = 1.0, resolution = 0.05)
        self.step_size_scale.pack ()
        self.play_var = IntVar ()
        self.play_button = Checkbutton (self.right_frame,
                variable = self.play_var, text = 'Play', command = self.play,
                underline = 0)
        self.play_button.pack ()
        self.bind_all ('p', lambda event: self.play_button.invoke ())

    def step (self):
        """Do a step.  Signal to the Hub we are ready to wait to the next step
        date."""
        self.node.wait_async (self.date
                + int (self.step_size_scale.get () * self.node.tick))
        self.synced = False
        self.step_after = None
        self.step_time = time.time ()

    def play (self):
        """Activate auto-steping."""
        if self.play_var.get ():
            if self.step_after is None and self.synced:
                self.step ()
            self.step_button.configure (state = DISABLED)
        else:
            if self.step_after is not None:
                self.after_cancel (self.step_after)
                self.step_after = None
            self.step_button.configure (state = NORMAL)

    def read (self, file, mask):
        """Handle event on the Node."""
        self.node.read ()
        if not self.synced and self.node.sync ():
            self.synced = True
            self.date = self.node.date
            self.now_label.configure (text = 'Now: %.2f s'
                    % (float (self.date) / self.node.tick))
            self.update ()
            if self.play_var.get ():
                assert self.step_after is None
                next = self.step_time + self.step_size_scale.get ()
                delay = next - time.time ()
                if delay > 0:
                    self.step_after = self.after (int (delay * 1000),
                            self.step)
                else:
                    self.step_after = self.after_idle (self.step)

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
