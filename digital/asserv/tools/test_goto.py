import sys
sys.path.append (sys.path[0] + '/../../../host')

from asserv import Asserv
import asserv.init
import proto.popen_io
import serial
import random

if sys.argv[1] == '!':
    io = proto.popen_io.PopenIO (sys.argv[2:])
    init = asserv.init.host
else:
    io = serial.Serial (sys.argv[1])
    init = asserv.init.target
a = Asserv (io, **init)
for i in xrange (10):
    x = random.randrange (2000)
    y = random.randrange (1100)
    a.goto (x, y)
    a.goto_angle (random.randrange (360))
a.goto (0, 0)
a.goto_angle (0)
a.close ()
