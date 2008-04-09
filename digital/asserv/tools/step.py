import sys
sys.path.append (sys.path[0] + '/../../../host/proto')

from asserv import Asserv
import popen_io
import Gnuplot

def step (name, kp, ki, kd, plots):
    io = popen_io.PopenIO (sys.argv[1:])
    a = Asserv (io, **{ name + 'kp': kp, name + 'ki': ki, name + 'kd': kd})
    a.stats ('PW')
    a.consign (name, 0x200)
    a.wait (lambda: a.stats_count > 225 * 2)
    list = a.get_stats ()

    g = Gnuplot.Gnuplot (persist = True)
    g ('set data style lines')
    g.plot (*[Gnuplot.Data (list, cols = i) for i in plots])

step ('t', 1, 0, 16, (0, 6, 7))
#step ('a', 1, 0, 16, (2, 6, 7))
#step ('a0', 1, 0, 16, (4, 8))
