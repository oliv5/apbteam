import sys
sys.path.append (sys.path[0] + '/../../../host/proto')

from asserv import Asserv
import popen_io
import serial
import Gnuplot

def step (name, kp, ki, kd, plots):
    if sys.argv[1] == '!':
	io = popen_io.PopenIO (sys.argv[2:])
    else:
	io = serial.Serial (sys.argv[1])
    a = Asserv (io, **{ name + 'kp': kp, name + 'ki': ki, name + 'kd': kd})
    a.stats (*plots)
    a.consign (name, 0x200)
    array = a.get_stats (225)
    a.close ()

    g = Gnuplot.Gnuplot (persist = True)
    g ('set data style lines')
    g.plot (*[array[:, i] for i in xrange (len (plots))])

step ('t', 1, 0, 16, ('te', 'lw', 'rw'))
#step ('a', 1, 0, 16, ('ae', 'lw', 'rw'))
#step ('a0', 1, 0, 16, ('a0e', 'a0w'))
