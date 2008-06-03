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
