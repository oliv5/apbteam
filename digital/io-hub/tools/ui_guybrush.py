# io-hub - Modular Input/Output. {{{
#
# Copyright (C) 2012 Nicolas Schodet
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
"""Tools to interface with guybrush io-hub."""
from Tkinter import *

import io_hub
from utils.init_proto import init_proto

import simu.robots.guybrush.model.bag
defs = simu.robots.guybrush.model.bag.Bag

class InterGuybrush (Frame):

    def __init__ (self, master = None):
        Frame.__init__ (self, master)
        self.pack (expand = True, fill = 'both')
        self.create_widgets ()
        self.current_value = 0
        self.update ()
        self.io = init_proto ('guybrush', io_hub.Proto)
        self.io.async = True
        def io_read (*args):
            self.io.proto.read ()
            self.io.proto.sync ()
        def io_timeout ():
            self.io.proto.sync ()
            self.after (100, io_timeout)
        self.tk.createfilehandler (self.io, READABLE, io_read)
        io_timeout ()

    def create_widgets (self):
        def out_button (name, mask):
            def command ():
                self.toggle (mask)
            Button (self, text = name, command = command).pack ()
        Button (self, text = 'Reset', command = self.reset).pack ()
        for i in xrange (10):
            out_button ('Toggle %d' % i, 1 << i)
        out_button ('Init', 1 << defs.OUTPUT_UPPER_CLAMP_UP
                | 1 << defs.OUTPUT_UPPER_CLAMP_IN
                | 1 << defs.OUTPUT_DOOR_CLOSE)
        out_button ('UClamp open', 1 << defs.OUTPUT_UPPER_CLAMP_OPEN)
        out_button ('UClamp in/out', 1 << defs.OUTPUT_UPPER_CLAMP_IN
                | 1 << defs.OUTPUT_UPPER_CLAMP_OUT)
        out_button ('UClamp up/down', 1 << defs.OUTPUT_UPPER_CLAMP_UP
                | 1 << defs.OUTPUT_UPPER_CLAMP_DOWN)
        out_button ('Door open/close', 1 << defs.OUTPUT_DOOR_OPEN
                | 1 << defs.OUTPUT_DOOR_CLOSE)
        out_button ('LClamp 1 open', 1 << defs.OUTPUT_LOWER_CLAMP_1_CLOSE)
        out_button ('LClamp 2 open', 1 << defs.OUTPUT_LOWER_CLAMP_2_CLOSE)
        self.current_label = Label (self, text = '')
        self.current_label.pack ()

    def toggle (self, mask):
        self.current_value = self.current_value ^ mask
        self.io.output (mask, 'toggle')
        self.update ()

    def update (self):
        text = [ ]
        for i in xrange (10):
            text.insert (0, str ((self.current_value >> i) & 1))
        self.current_label.configure (text = ''.join (text))

    def reset (self):
        self.io.reset ()
        self.current_value = 0
        self.update ()

if __name__ == '__main__':
    app = InterGuybrush ()
    app.mainloop ()
