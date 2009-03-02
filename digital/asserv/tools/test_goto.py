import sys
import random
import math

import asserv
import asserv.init
import proto.popen_io
import serial

if sys.argv[1] == '!':
    io = proto.popen_io.PopenIO (sys.argv[2:])
    init = asserv.init.host
else:
    io = serial.Serial (sys.argv[1])
    init = asserv.init.target
a = asserv.Proto (io, **init)
for i in xrange (10):
    x = random.randrange (2000)
    y = random.randrange (1100)
    a.goto (x, y)
    a.goto_angle (math.radians (random.randrange (360)))
a.goto (0, 0)
a.goto_angle (0)
a.close ()
