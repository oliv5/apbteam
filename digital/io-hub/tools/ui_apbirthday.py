# io-hub - Modular Input/Output. {{{
#
# Copyright (C) 2013 Nicolas Schodet
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
"""Tools to interface with apbirthday io-hub."""
from Tkinter import *

import io_hub
from utils.init_proto import init_proto

import io_hub.apbirthday

class InterAPBirthday (Frame):

    def __init__ (self, master = None):
        Frame.__init__ (self, master)
        self.pack (expand = True, fill = 'both')
        self.create_widgets ()
        self.current_value = 0
        self.update ()
        self.io = init_proto ('apbirthday', io_hub.Proto)
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
        def out_button (name, *toggle):
            def command ():
                self.toggle (io_hub.apbirthday.output_mask (*toggle))
            Button (self, text = name, command = command).pack ()
        Button (self, text = 'Reset', command = self.reset).pack ()
        for i, n in enumerate (io_hub.apbirthday.outputs):
            out_button ('Toggle %d: %s' % (i, n), n)
        out_button ('Init', 'cake_arm_out', 'cake_push_far_out',
                'cake_push_near_out')
        out_button ('Arm in/out', 'cake_arm_in', 'cake_arm_out')
        out_button ('Push far in/out', 'cake_push_far_in', 'cake_push_far_out')
        out_button ('Push near in/out', 'cake_push_near_in', 'cake_push_near_out')
        out_button ('Plate clamp', 'cherry_plate_clamp')
        out_button ('Plate up/down', 'cherry_plate_up', 'cherry_plate_down')
        out_button ('Cherry bad in/out', 'cherry_bad_in', 'cherry_bad_out')
        self.current_label = Label (self, text = '')
        self.current_label.pack ()

    def toggle (self, mask):
        self.current_value = self.current_value ^ mask
        self.io.output (mask, 'toggle')
        self.update ()

    def update (self):
        text = [ ]
        for i in xrange (len (io_hub.apbirthday.outputs)):
            text.insert (0, str ((self.current_value >> i) & 1))
        self.current_label.configure (text = ''.join (text))

    def reset (self):
        self.io.reset ()
        self.current_value = 0
        self.update ()

if __name__ == '__main__':
    app = InterAPBirthday ()
    app.mainloop ()
