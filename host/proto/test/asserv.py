import sys
sys.path.append (sys.path[0] + '/..')

import proto
from fio import IO
import time, select, os

# Pass program name as argument.
fout, fin = os.popen2 (sys.argv[1:], 't', 1)
time.sleep (0.5)
io = IO (fin, fout)

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

while not p.sync () or done != 3:
    fds = select.select ((p,), (), (), 0.1)[0]
    for i in fds:
	i.read ()
