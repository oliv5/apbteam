import random
import math

import asserv
import asserv.init
from utils.init_proto import init_proto

a = init_proto (None, asserv.Proto, asserv.init)
for i in xrange (10):
    x = random.randrange (2000)
    y = random.randrange (1100)
    a.goto (x, y)
    a.goto_angle (math.radians (random.randrange (360)))
a.goto (0, 0)
a.goto_angle (0)
a.close ()
