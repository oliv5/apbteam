import asserv
import asserv.init
import io_hub
import io_hub.init

from proto.popen_io import PopenIO
import math
import subprocess

class Robot:
    """APBirthday robot instance."""

    import simu.model.table_eurobot2013 as table_model
    import simu.view.table_eurobot2013 as table_view

    import simu.robots.apbirthday.link.bag as robot_link
    import simu.robots.apbirthday.model.bag as robot_model
    import simu.robots.apbirthday.view.bag as robot_view

    # TODO: use right position.
    robot_start_pos = {
            False: (250, 2000 - 250, math.radians (0)),
            True: (3000 - 250, 2000 - 250, math.radians (180))
            }

    client_nb = 3

    def __init__ (self, proto_time, instance = 'robot0'):
        self.instance = instance
        def proto (proto_class, cmd, init):
            cmd = [ s.format (instance = instance) for s in cmd ]
            return proto_class (PopenIO (cmd), proto_time, **init)
        def prog (cmd):
            cmd = [ s.format (instance = instance) for s in cmd ]
            subprocess.Popen (cmd)
        asserv_cmd = ('../../mimot/src/asserv/asserv.host',
                '-i{instance}:asserv0', '-m9', 'apbirthday')
        io_hub_cmd = ('../../io-hub/src/apbirthday/apbirthday.host',
                '-i{instance}:io0')
        beacon_stub_cmd = ('../../beacon/src/stub/simu_stub.host',
                '-i{instance}:beacon0')
        self.asserv = proto (asserv.Proto, asserv_cmd,
                asserv.init.host['apbirthday'])
        self.io = proto (io_hub.ProtoGuybrush, io_hub_cmd,
                io_hub.init.host['apbirthday'])
        self.beacon_stub = prog (beacon_stub_cmd)
        self.protos = (self.asserv, self.io)

