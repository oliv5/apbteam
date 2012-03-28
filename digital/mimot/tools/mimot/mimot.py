# mimot - Mini motor control, with motor driver. {{{
#
# Copyright (C) 2010 Nicolas Schodet
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
"""Proto interface to mimot."""

import time
import numpy
import math

import proto
from utils.observable import Observable

class Proto:
    """Provide functions to communicate with mimot using the proto
    interface."""

    stats_format = {
            'C': 'HH',
            'S': 'bb',
            'P': 'hhhh',
            'W': 'hh',
            }
    # The last occuring stats will increment stats_count, so they have to
    # be in the same order than in mimot program.
    stats_order = 'CSPW'
    stats_items = {
            'a0c': ('C', 0),
            'a1c': ('C', 1),
            'a0s': ('S', 0),
            'a1s': ('S', 1),
            'a0e': ('P', 0),
            'a0i': ('P', 1),
            'a1e': ('P', 2),
            'a1i': ('P', 3),
            'a0w': ('W', 0),
            'a1w': ('W', 1),
            }

    _index = dict (a0 = 0, a1 = 1)

    def __init__ (self, file, time = time.time, **param):
        """Initialise communication and send parameters to asserv."""
        self.proto = proto.Proto (file, time, 0.1)
        self.async = False
        self.aseq = [ 0, 0 ]
        self.aseq_ack = [ 0, 0 ]
        self.proto.register ('A', 'BB', self.__handle_ack)
        def make_handle (s):
            return lambda *args: self.__handle_stats (s, *args)
        for (s, f) in self.stats_format.iteritems ():
            self.proto.register (s, f, make_handle (s))
        self.stats_enabled = None
        self.param = dict (
                a0_kp = 0, a0_ki = 0, a0_kd = 0,
                a0_acc = 1, a0_speed_max = 0, a0_speed_slow = 0,
                a0_bd_error_limit = 2048, a0_bd_speed_limit = 0x10, a0_bd_counter_limit = 20,
                a0_reverse = False,
                a1_kp = 0, a1_ki = 0, a1_kd = 0,
                a1_acc = 1, a1_speed_max = 0, a1_speed_slow = 0,
                a1_bd_error_limit = 2048, a1_bd_speed_limit = 0x10, a1_bd_counter_limit = 20,
                a1_reverse = False,
                e_sat = 1023, i_sat = 1023, d_sat = 1023,
                )
        self.param.update (param)
        self.send_param ()

    def stats (self, *stats_items, **options):
        """Activate stats.  Take a list of items to record, and an optional
        interval option."""
        interval = 1
        if 'interval' in options:
            interval = options['interval']
        # Build list of stats letters.
        stats = [self.stats_items[i][0] for i in stats_items]
        stats = [s for s in self.stats_order if s in stats]
        stats_last_pos = 0
        stats_pos = { }
        for s in stats:
            stats_pos[s] = stats_last_pos
            stats_last_pos += len (self.stats_format[s])
        # Build stats item positions.
        self.stats_items_pos = [ ]
        for i in stats_items:
            id = self.stats_items[i]
            self.stats_items_pos.append (stats_pos[id[0]] + id[1])
        # Enable stats.
        for s in stats:
            self.proto.send (s, 'B', interval)
        # Prepare aquisition.
        self.stats_enabled = stats
        self.stats_counter = stats[-1]
        self.stats_count = 0
        self.stats_list = [ ]
        self.stats_line = [ ]

    def get_stats (self, wait = None):
        """Get recorded stats.  Return an array with every requested stats."""
        if wait:
            self.wait (wait)
        list = self.stats_list
        # Drop first line as it might be garbage.
        del list[0]
        for s in reversed (self.stats_enabled):
            self.proto.send (s, 'B', 0)
        # Extract asked stats.
        array = numpy.array (list)
        array = array[:, self.stats_items_pos]
        # Cleanup.
        self.stats_enabled = None
        del self.stats_items_pos
        del self.stats_counter
        del self.stats_count
        del self.stats_list
        del self.stats_line
        return array

    def consign (self, w, c):
        """Consign offset."""
	self.proto.send ('c', 'Bh', self._index[w], c)

    def speed (self, w, s):
        """Speed consign."""
	self.proto.send ('s', 'Bb', self._index[w], s)

    def speed_pos (self, w, offset):
        """Speed controlled position consign."""
	i = self._index[w]
	self.aseq[i] += 1
	self.proto.send ('s', 'BlB', i, offset, self.aseq[i])
        self.wait (self.finished, auto = True)

    def goto_pos (self, w, pos):
        """Go to absolute position."""
	i = self._index[w]
	self.aseq[i] += 1
	self.proto.send ('y', 'BhB', i, pos, self.aseq[i])
        self.wait (self.finished, auto = True)

    def clamp (self, w, s, pwm):
        """Clamp (speed control, then open loop PWM)."""
	i = self._index[w]
	self.aseq[i] += 1
	self.proto.send ('y', 'BBhB', i, s, pwm, self.aseq[i])
        self.wait (self.finished, auto = True)

    def send_param (self):
        """Send all parameters."""
        p = self.param
        def f88 (x):
            return int (round (x * (1 << 8)))
        for m in ('a0', 'a1'):
            index = self._index [m]
            self.proto.send ('p', 'cBH', 'p', index, f88 (p[m + '_kp']))
            self.proto.send ('p', 'cBH', 'i', index, f88 (p[m + '_ki']))
            self.proto.send ('p', 'cBH', 'd', index, f88 (p[m + '_kd']))
            self.proto.send ('p', 'cBH', 'a', index, f88 (p[m + '_acc']))
            self.proto.send ('p', 'cBBB', 's', index, p[m + '_speed_max'],
                    p[m + '_speed_slow'])
            self.proto.send ('p', 'cBHHB', 'b', index,
                    p[m + '_bd_error_limit'], p[m + '_bd_speed_limit'],
                    p[m + '_bd_counter_limit'])
        self.proto.send ('p', 'cH', 'E', p['e_sat'])
        self.proto.send ('p', 'cH', 'I', p['i_sat'])
        self.proto.send ('p', 'cH', 'D', p['d_sat'])
        reverse = 0
        for i, m in enumerate (('a0', 'a1')):
            if p[m + '_reverse']:
                reverse |= 1 << i
        self.proto.send ('p', 'cB', 'w', reverse)

    def write_eeprom (self):
        """Request an EEPROM write."""
        self.proto.send ('p', 'cB', 'E', 1)
        time.sleep (1)
        self.wait (lambda: True)

    def __handle_stats (self, stat, *args):
        """Record received stats."""
        if self.stats_enabled is not None:
            self.stats_line.extend (args)
            if self.stats_counter == stat:
                self.stats_list.append (self.stats_line)
                self.stats_line = [ ]
                self.stats_count += 1

    def __handle_ack (self, a0seq, a1seq):
        """Record current acknowledge level and acknowledge reception."""
        self.aseq_ack[0] = a0seq & 0x7f
        self.aseq_ack[1] = a1seq & 0x7f
        self.proto.send ('a', 'BB', a0seq, a1seq)

    def wait (self, cond = None, auto = False):
        """Wait for a condition to become true, or for a number of recorded
        statistics.  If auto is True, do not wait in asynchronous mode."""
        if auto and self.async:
            return
        try:
            cond_count = int (cond)
            cond = lambda: self.stats_count > cond_count
        except TypeError:
            pass
        self.proto.wait (cond)

    def finished (self):
        """Return True if movement commands have been acknowledged."""
        return (self.aseq[0] == self.aseq_ack[0]
                and self.aseq[1] == self.aseq_ack[1])

    def free (self):
        """Coast motors."""
        self.proto.send ('w')

    def reset (self):
        """Coast all motors and reset asserv."""
        self.proto.send ('w')
        self.proto.send ('z')
        self.proto.send ('z')

    def close (self):
        """Gracefully close communications."""
        self.reset ()
        self.wait (lambda: True)
        self.proto.file.close ()

    def fileno (self):
        """Return fileno for select() calls."""
        return self.proto.fileno ()

