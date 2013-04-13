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
        Button (self, text = 'Reset', command = self.reset).pack ()
        # Outputs.
        output_frame = Frame (self)
        output_frame.pack ()
        def make_setreset (index):
            def setreset ():
                val = self.output_var[index].get ()
                self.io.output (1 << index, val)
            return setreset
        def make_toggle (*index):
            def toggle ():
                mask = 0
                for i in index:
                    var = self.output_var[i]
                    val = 1 - var.get ()
                    var.set (val)
                    mask |= 1 << i
                self.io.output (mask, 'toggle')
            return toggle
        def common (a, b):
            if a is None or b is None: return None
            a1, a2 = a.rsplit ('_', 1)
            b1, b2 = b.rsplit ('_', 1)
            if a1 != b1: return None
            def cmp (a, b):
                return (a == 'up' and b == 'down'
                        or a == 'in' and b == 'out'
                        or a == 'open' and b == 'close')
            if cmp (a2, b2) or cmp (b2, a2):
                return a1
            return None
        previous = None
        self.output_var = [ ]
        for i, name in enumerate (io_hub.apbirthday.outputs):
            var = IntVar ()
            self.output_var.append (var)
            button = Checkbutton (output_frame, indicatoron = 0,
                    text = '%d: %s' % (i, name), command = make_setreset (i),
                    variable = var)
            button.grid (column = 0, row = i, sticky = 'nsew')
            c = common (name, previous)
            if c:
                button = Button (output_frame, text = c, command =
                        make_toggle (i - 1, i))
                button.grid (column = 1, row = i - 1, rowspan = 2,
                        sticky = 'nsew')
            previous = name

    def reset (self):
        mask = 0
        for i in xrange (len (io_hub.apbirthday.outputs)):
            self.output_var[i].set (0)
            mask |= 1 << i
        self.io.output (mask, 'clear')

if __name__ == '__main__':
    app = InterAPBirthday ()
    app.mainloop ()
