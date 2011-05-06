import simu.model.table_eurobot2011
import simu.view.table_eurobot2011

import simu.robots.robospierre.link.bag
import simu.robots.robospierre.model.bag
import simu.robots.robospierre.view.bag

import asserv
import asserv.init
import mimot
import mimot.init
import io
import io.init

from proto.popen_io import PopenIO
import math

class Robot:

    def __init__ (self, proto_time):
        self.table_model = simu.model.table_eurobot2011
        self.table_view = simu.view.table_eurobot2011
        self.robot_link = simu.robots.robospierre.link.bag
        self.robot_model = simu.robots.robospierre.model.bag
        self.robot_view = simu.robots.robospierre.view.bag
        asserv_cmd = ('../../asserv/src/asserv/asserv.host', '-m9', 'marcel')
        mimot_cmd = ('../../mimot/src/dirty/dirty.host', '-m9', 'marcel')
        io_cmd = ('../../io/src/io.host')
        self.asserv = asserv.Proto (PopenIO (asserv_cmd), proto_time,
                **asserv.init.host)
        self.mimot = mimot.Proto (PopenIO (mimot_cmd), proto_time,
                **mimot.init.host)
        self.io = io.Proto (PopenIO (io_cmd), proto_time,
                **io.init.host)
        self.robot_start_pos = {
                False: (700, 2100 - 250, math.radians (-270)),
                True: (3000 - 700, 2100 - 250, math.radians (-270))
                }

