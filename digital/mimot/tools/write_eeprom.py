import sys

import mimot
import mimot.init
import proto.popen_io
import serial

if sys.argv[1] == '!':
    io = proto.popen_io.PopenIO (sys.argv[2:])
    init = mimot.init.host
else:
    io = serial.Serial (sys.argv[1])
    init = mimot.init.target
a = mimot.Proto (io, **init)
a.write_eeprom ()
a.close ()
