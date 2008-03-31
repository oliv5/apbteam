import sys
sys.path.append (sys.path[0] + '/../../../../../host/mex')

from mex.hub import Hub
from mex.msg import Msg

import os, signal

def log (x):
    print x

h = Hub (min_clients = 2, log = log)

pid1 = os.spawnl (os.P_NOWAIT, './test_mex.host', './test_mex.host', '1')
pid2 = os.spawnl (os.P_NOWAIT, './test_mex.host', './test_mex.host', '2')

try:
    h.wait ()
finally:
    os.kill (pid1, signal.SIGTERM)
    os.waitpid (pid1, 0)
    os.kill (pid2, signal.SIGTERM)
    os.waitpid (pid2, 0)
    import time
    time.sleep (1)
