# io-hub - Modular Input/Output. {{{
#
# Copyright (C) 2011 Nicolas Schodet
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
import proto, time

class Proto:

    def __init__ (self, file, time = time.time, **param):
        self.proto = proto.Proto (file, time, 0.1)
        self.async = False
        self.param = param
        self.send_param ()

    def send_param (self):
        pass

    def reset (self):
        self.proto.send ('z')
        self.proto.send ('z')

    def pwm_set (self, index, value):
        self.proto.send ('w', 'Bh', index, value)

    def pwm_set_timed (self, index, value, time, rest_value):
        self.proto.send ('w', 'BhHh', index, value, time, rest_value)

    def clamp_move (self, pos):
        self.proto.send ('c', 'B', pos)

    def close (self):
        self.reset ()
        self.proto.wait (lambda: True)
        self.proto.file.close ()

    def fileno (self):
        return self.proto.fileno ()

