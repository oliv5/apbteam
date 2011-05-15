import random
import math

import asserv
import asserv.init
from utils.init_proto import InitProto

ip = InitProto (None, asserv.Proto, asserv.init)
ip.parser.add_option ('-i', '--iterations',
        help = "number of test iterations", metavar = 'NB', default = 10)
ip.parser.add_option ('-t', '--type',
        help = "test type, one of random or linear", metavar = 'TYPE',
        default = 'random')
ip.parse_args ()
a = ip.get_proto ()
try:
    if ip.options.type == 'random':
        for i in xrange (ip.options.iterations):
            x = random.randrange (2000)
            y = random.randrange (1100)
            a.goto (x, y)
            a.goto_angle (math.radians (random.randrange (360)))
        a.goto (0, 0)
        a.goto_angle (0)
    elif ip.options.type == 'linear':
        for i in xrange (ip.options.iterations):
            a.speed_pos ('t', 1000)
            a.speed_pos ('t', -1000)
except:
    pass
a.close ()
