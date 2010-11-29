import os, signal, time

from mex.hub import Hub
from mex.msg import Msg
from mex.node import Node
from utils.forked import Forked

h = Hub (min_clients = 3)
fh = Forked (h.wait)

n = Node ()
def nf ():
    while True:
        time.sleep (1)
        n.wait (n.date + 1000)
fn = Forked (nf)

slave = os.popen ('./slave/test_twi_slave.host', 'w')

try:
    os.system ('./master/test_twi_master.host')
finally:
    fn.kill ()
    fh.kill ()
    slave.close ()
    time.sleep (1)
