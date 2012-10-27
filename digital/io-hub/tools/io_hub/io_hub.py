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

__all__ = [ 'Proto', 'ProtoRobospierre', 'ProtoGuybrush' ]

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

    def fsm_debug (self):
        self.proto.send ('f')

    def pwm_set (self, index, value):
        self.proto.send ('w', 'Bh', index, value)

    def pwm_set_timed (self, index, value, time, rest_value):
        self.proto.send ('w', 'BhHh', index, value, time, rest_value)

    def goto (self, x, y, backward):
        self.proto.send ('m', 'hhB', x, y, backward)

    def output (self, mask, action):
        m = { 'clear': 0, 'set': 1, 'toggle': 2 }
        if action in m:
            action = m[action]
        self.proto.send ('o', 'Lb', mask, action)

    def output_transient (self, mask, duration):
        self.proto.send ('o', 'LH', mask, duration)

    def close (self):
        self.reset ()
        self.proto.wait (lambda: True)
        self.proto.file.close ()

    def fileno (self):
        return self.proto.fileno ()

class ProtoRobospierre (Proto):

    def clamp_move (self, pos):
        self.proto.send ('c', 'B', pos)

    def clamp_move_element (self, from_, to):
        self.proto.send ('c', 'BB', from_, to)

    def drop (self, order):
        if order == 'drop_clear':
            self.proto.send ('d', 'B', 0x00)
        elif order == 'drop_forward':
            self.proto.send ('d', 'B', 0x01)
        elif order == 'drop_backward':
            self.proto.send ('d', 'B', 0x02)
        else:
            raise ValueError

    def door (self, pos, open_):
        self.proto.send ('d', 'BB', pos, (0, 1)[open_])

    def clamp_openclose (self, open_):
        self.proto.send ('d', 'BB', 0xff, (0, 1)[open_])

class ProtoGuybrush (Proto):

    def tree_detected(self):
        self.proto.send('t')
    def stop_tree_approach(self):
        self.proto.send('s')
    def empty_tree(self):
        self.proto.send('e')
    def robot_is_back(self):
        self.proto.send('r')
    def unblock_bottom_clamp(self):
        self.proto.send('u')
