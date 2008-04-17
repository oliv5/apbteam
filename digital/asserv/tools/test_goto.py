import sys
sys.path.append (sys.path[0] + '/../../../host/proto')

from asserv import Asserv
import init
import popen_io
import serial
import random

if sys.argv[1] == '!':
    io = popen_io.PopenIO (sys.argv[2:])
else:
    io = serial.Serial (sys.argv[1])
a = Asserv (io, **init.host)
for i in xrange (10):
    x = random.randrange (2000)
    y = random.randrange (1100)
    a.goto (x, y)
    a.goto_angle (random.randrange (360))
a.goto (0, 0)
a.goto_angle (0)
a.close ()
