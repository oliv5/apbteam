# mex - Message Exchange library. {{{
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
import select

from mex.hub import Hub
from mex.node import Node
from mex.msg import Msg
from utils.forked import Forked

def log (x):
    print x

h = Hub (min_clients = 2, log = log)

def c1 ():
    n = Node ()
    mtype_coucou = n.reserve ('coucou')
    def a (msg):
        print 'oucouc'
        nb, = msg.pop ('B')
        nb += 1
        m = Msg (msg.mtype)
        m.push ('B', nb)
        n.response (m)
    n.register ('oucouc', a)
    def b ():
        assert False
    eb = n.schedule (31, b)
    def c ():
        print 'hello'
        n.cancel (eb)
    n.schedule (28, c)
    m = Msg (mtype_coucou)
    n.send (m)
    n.wait ()

f1 = Forked (c1)

def c2 ():
    n = Node ()
    mtype_oucouc = n.reserve ('oucouc')
    mtype_coucou = n.reserve ('coucou')
    def a (msg):
        print 'coucou'
    n.register (mtype_coucou, a)
    m = Msg (mtype_oucouc)
    m.push ('B', 42)
    r = n.request (m)
    assert r.mtype == mtype_oucouc
    assert r.pop ('B') == (43,)
    n.wait_async (42)
    while not n.sync ():
        fds = select.select ((n, ), (), ())[0]
        for i in fds:
            i.read ()
    n.wait ()

f2 = Forked (c2)

try:
    h.wait ()
finally:
    f1.kill ()
    f2.kill ()
    import time
    time.sleep (1)
