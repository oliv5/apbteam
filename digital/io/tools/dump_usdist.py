import sys

import io
import io.init
import serial

fd = serial.Serial (sys.argv[1])
p = io.Proto (fd, **io.init.target)
def cb (*val):
    l = [ ]
    for v in val:
        if v is None:
            l.append ('___')
        else:
            l.append (v)
    print l
try:
    p.stats_usdist (cb)
except KeyboardInterrupt:
    pass
p.close ()
