import sys
import time
import os

from lib.traceclass import *

templatedir = os.path.split (__file__)[0]

class Writer:

    """Template writer"""
    def __init__ (self, name, enum_name):
        self.__name = name
        self.__enum_name = enum_name

    def parse_list (self, event_list = None):
        if event_list != None:
            string = ""
            for i in range (0, len (event_list)):
                string += "    TRACE_" + event_list[i].name() + ",\n"
            string += "    TRACE_NB"
            return string.upper()

    def __read_template__ (self, string):
        f = open (templatedir + '/template.h', 'r')
        template = f.read()
        f.close()

        if self.__name != None:
            define = self.__name.replace('.', '_')
        else:
            define = "trace.h"
        template = template.replace('%%template%%', define)

        if self.__enum_name == None:
            template = template.replace('%%enum_name%%', "trace_id_t")
        else:
            template = template.replace('%%enum_name%%', self.__enum_name)

        template = template.replace('%%data%%', string)
        template = template.replace('%%year%%', time.strftime ('%Y'))
        return template


    def write_file (self, string):
        template = self.__read_template__(string)
        f = open (self.__name, 'w')
        f.write (template)
        f.close()

    def print_file (self, string):
        template = self.__read_template__(string)
        print template
