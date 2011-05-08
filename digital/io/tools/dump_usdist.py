import io
import io.init
from utils.init_proto import init_proto

p = init_proto ('marcel', io.Proto, io.init)
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
