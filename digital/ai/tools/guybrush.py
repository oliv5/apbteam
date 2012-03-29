import asserv
import asserv.init
import mimot
import mimot.init
import io_hub
import io_hub.init

from proto.popen_io import PopenIO
import math

class Robot:
    """Guybrush robot instance."""

    import simu.model.table_eurobot2012 as table_model
    import simu.view.table_eurobot2012 as table_view

    import simu.robots.guybrush.link.bag as robot_link
    import simu.robots.guybrush.model.bag as robot_model
    import simu.robots.guybrush.view.bag as robot_view

    robot_start_pos = {
            False: (300, 2000 - 200, math.radians (180)),
            True: (3000 - 300, 2000 - 200, math.radians (0))
            }

    client_nb = 3

    def __init__ (self, proto_time, instance = 'robot0'):
        self.instance = instance
        def proto (proto_class, cmd, init):
            cmd = [ s.format (instance = instance) for s in cmd ]
            return proto_class (PopenIO (cmd), proto_time, **init)
        asserv_cmd = ('../../asserv/src/asserv/asserv.host',
                '-i{instance}:asserv0', '-m9', 'robospierre')
        mimot_cmd = ('../../mimot/src/dirty/dirty.host',
                '-i{instance}:mimot0', '-m9', 'guybrush')
        io_hub_cmd = ('../../io-hub/src/guybrush/io_hub.host',
                '-i{instance}:io0')
        self.asserv = proto (asserv.Proto, asserv_cmd,
                asserv.init.host['robospierre'])
        self.mimot = proto (mimot.Proto, mimot_cmd,
                mimot.init.host['guybrush'])
        self.io = proto (io_hub.ProtoGuybrush, io_hub_cmd,
                io_hub.init.host['guybrush'])
        self.protos = (self.asserv, self.mimot, self.io)

