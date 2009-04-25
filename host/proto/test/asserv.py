# proto - Proto interface. {{{
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
import sys
import time, select, os

import proto
from proto.popen_io import PopenIO

# Pass program name as argument.
io = PopenIO (sys.argv[1:])

def log (x):
    print x

p = proto.Proto (io, time.time, 0.5, log)

done = 0

def reset ():
    print 'reset'

def counter_stat (left, right, aux0):
    print 'counter %u, %u, %u' % (left, right, aux0)
    global done
    done += 1

p.register ('z', '', reset)
p.register ('C', 'HHH', counter_stat)

p.send ('C', 'B', 255)

p.wait (lambda: done == 3)
