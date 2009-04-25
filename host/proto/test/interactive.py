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
sys.path.append (sys.path[0] + '/../..')

import proto
from fio import IO
import time, select

def log (x):
    print x

p = proto.Proto (IO (), time.time, 2, log)

def a (i, j):
    print 'a (%d, %d)' % (i, j)

def b (i):
    print 'b (%d)' % i

p.register ('a', 'BH', a)
p.register ('b', 'L', b)

p.send ('a', 'BH', 1, 2)
p.send ('b', 'L', 3)

while True:
    p.sync ()
    fds = select.select ((p,), (), (), 0.1)[0]
    for i in fds:
        i.read ()
