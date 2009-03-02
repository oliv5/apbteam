import sys

from asserv import Asserv
import proto.popen_io
import serial
import Gnuplot

def step (name, offset, kp, ki, kd, plots, **param):
    if sys.argv[1] == '!':
        io = proto.popen_io.PopenIO (sys.argv[2:])
    else:
        io = serial.Serial (sys.argv[1])
    p = { name + 'kp': kp, name + 'ki': ki, name + 'kd': kd}
    p.update (param)
    a = Asserv (io, **p)
    a.stats (*plots)
    a.consign (name, offset)
    #a.speed (name, 16)
    array = a.get_stats (225)
    a.close ()

    g = Gnuplot.Gnuplot (persist = True)
    g ('set data style lines')
    g.plot (*[array[:, i] for i in xrange (len (plots))])

step ('t', 0x200, 1, 0, 16, ('te', 'lw', 'rw'))
#step ('a', 0x200, 1, 0, 16, ('ae', 'lw', 'rw'))
#step ('a0', 100, 0.8, 0.05, 0.05, ('a0e', 'a0w', 'a0i'), I = 8191)
