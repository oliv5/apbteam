import asserv
import asserv.init
import mimot
import mimot.init
import io
import io.init

from proto.popen_io import PopenIO
import math

class Robot:
    """Marcel robot instance."""

    import simu.model.table_eurobot2010 as table_model
    import simu.view.table_eurobot2010 as table_view

    import simu.robots.marcel.link.bag as robot_link
    import simu.robots.marcel.model.bag as robot_model
    import simu.robots.marcel.view.bag as robot_view

    robot_start_pos = {
            False: (300, 2100 - 305, math.radians (-270)),
            True: (3000 - 300, 2100 - 305, math.radians (-270))
            }

    client_nb = 3

    tick = 900

    def __init__ (self, proto_time, instance = 'robot0'):
        self.instance = instance
        def proto (proto_class, cmd, init):
            cmd = [ s.format (instance = instance) for s in cmd ]
            return proto_class (PopenIO (cmd), proto_time, **init)
        asserv_cmd = ('../../asserv/src/asserv/asserv.host',
                '-i{instance}:asserv0', '-m9', 'marcel')
        mimot_cmd = ('../../mimot/src/dirty/dirty.host',
                '-i{instance}:mimot0', '-m9', 'marcel')
        io_cmd = ('../../io/src/io.host', '-i{instance}:io0',
                '--ang-print-transitions')
        self.asserv = proto (asserv.Proto, asserv_cmd,
                asserv.init.host['marcel'])
        self.mimot = proto (mimot.Proto, mimot_cmd,
                mimot.init.host['marcel'])
        self.io = proto (io.Proto, io_cmd,
                io.init.host['marcel'])
        self.protos = (self.asserv, self.mimot, self.io)

