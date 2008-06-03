import sys
sys.path.append (sys.path[0] + '/../..')

from mex.hub import Hub
from mex.node import Node
from mex.msg import Msg
from utils.forked import Forked
import select

def log (x):
    print x

h = Hub (min_clients = 2, log = log)

def c1 ():
    n = Node ()
    def a (msg):
	print 'oucouc'
	nb, = msg.pop ('B')
	nb += 1
	m = Msg (msg.mtype)
	m.push ('B', nb)
	n.response (m)
    n.register (0x82, a)
    m = Msg (0x81)
    n.send (m)
    n.wait ()

f1 = Forked (c1)

def c2 ():
    n = Node ()
    def a (msg):
	print 'coucou'
    n.register (0x81, a)
    m = Msg (0x82)
    m.push ('B', 42)
    r = n.request (m)
    assert r.mtype == 0x82
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
