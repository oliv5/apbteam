import sys

import asserv
import asserv.init
from utils.init_proto import init_proto

a = init_proto (None, asserv.Proto, asserv.init)
a.write_eeprom ()
a.close ()
