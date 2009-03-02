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
