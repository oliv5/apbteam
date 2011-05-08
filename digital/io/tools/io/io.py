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
import proto, time

class Proto:

    def __init__ (self, file, time = time.time, **param):
        self.proto = proto.Proto (file, time, 0.1)
        self.async = False
        self.param = param
        self.send_param ()
        self.proto.register ('U', 'HHHH', self.__handle_usdist)

    def send_param (self):
        p = self.param
        if ('servo_pos') in p:
            for i, t in enumerate (p['servo_pos']):
                self.proto.send ('p', 'BBB', i, *t)

    def write_eeprom (self):
        self.proto.send ('p', 'cc', 'E', 's')
        time.sleep (1)
        self.proto.wait (lambda: True)

    def reset (self):
        self.proto.send ('z')
        self.proto.send ('z')

    def servo_pos (self, servo, pos):
        self.proto.send ('P', 'BB', servo, pos)

    def servo (self, servo, high_time):
        self.proto.send ('s', 'BB', servo, high_time)

    def pwm_set (self, value, timer):
        self.proto.send ('w', 'hH', value, timer)

    def loader (self, command):
        self.proto.send ('c', 'c', command)

    def stats_usdist (self, cb):
        self.proto.send ('U', 'B', 2 * 4 * 2)
        self.usdist_cb = cb
        while True:
            self.proto.wait ()

    def __handle_usdist (self, *val):
        l = [ ]
        for v in val:
            if v == 0xffff:
                l.append (None)
            else:
                l.append (v)
        self.usdist_cb (*l)

    def close (self):
        self.reset ()
        self.proto.wait (lambda: True)
        self.proto.file.close ()

    def fileno (self):
        return self.proto.fileno ()

