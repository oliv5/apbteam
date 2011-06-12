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
"""Generic switch."""
from Tkinter import *

class Switch:

    def __init__ (self, frame, model, text):
        self.var = IntVar ()
        if model.state is not None:
            self.var.set ((0, 1)[model.state])
        self.button = Checkbutton (frame, variable = self.var,
                command = self.__update, text = text, indicatoron = False)
        self.button.pack ()
        self.model = model
        self.__update ()

    def __update (self):
        self.model.state = self.var.get () != 0
        self.model.notify ()

