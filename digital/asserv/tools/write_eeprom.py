import sys
sys.path.append (sys.path[0] + '/../../../host')

from asserv import Asserv
import asserv.init
import proto.popen_io
import serial

if sys.argv[1] == '!':
    io = proto.popen_io.PopenIO (sys.argv[2:])
    init = asserv.init.host
else:
    io = serial.Serial (sys.argv[1])
    init = asserv.init.target
a = Asserv (io, **init)
a.write_eeprom ()
a.close ()
