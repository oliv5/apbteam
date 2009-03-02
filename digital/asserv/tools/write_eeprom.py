import sys

import asserv
import asserv.init
import proto.popen_io
import serial

if sys.argv[1] == '!':
    io = proto.popen_io.PopenIO (sys.argv[2:])
    init = asserv.init.host
else:
    io = serial.Serial (sys.argv[1])
    init = asserv.init.target
a = asserv.Proto (io, **init)
a.write_eeprom ()
a.close ()
