import sys
sys.path.append (sys.path[0] + '/../../../../../host')

from mex.hub import Hub
from mex.msg import Msg
from mex.node import Node
from utils.forked import Forked

import os, signal, time

def log (x):
    print x

h = Hub (min_clients = 3, log = log)
fh = Forked (h.wait)

n = Node ()
def nf ():
    while True:
        time.sleep (1)
        n.wait (n.date + 1)
fn = Forked (nf)

slave = os.popen ('./test_twi_sl.host', 'w')

try:
    os.system ('./test_twi_master.host')
finally:
    fn.kill ()
    fh.kill ()
    slave.close ()
    time.sleep (1)
