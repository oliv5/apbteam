import mimot
import mimot.init
from utils.init_proto import init_proto

m = init_proto (None, mimot.Proto, mimot.init)
m.write_eeprom ()
m.close ()
