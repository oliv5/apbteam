import sys
sys.path.append (sys.path[0] + '/../../../host/proto')

from asserv import Asserv
import init
import popen_io
import serial

if sys.argv[1] == '!':
    io = popen_io.PopenIO (sys.argv[2:])
    init = init.host
else:
    io = serial.Serial (sys.argv[1])
    init = init.target
a = Asserv (io, **init)
a.write_eeprom ()
a.close ()
