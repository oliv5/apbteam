import sys
import time
import os

from lib.traceclass import *

templatedir = os.path.split (__file__)[0]

class Writer:

    """Template writer"""
    def __init__ (self):
        self.__header_name = "events.h"
        self.__file_name = "events.host.c"

    def parse_event_identifiers (self, event_list = None):
        """Parse the event list and construct a string to embedded in a
        header file."""
        if event_list != None:
            string = ""
            prefix = "    "
            for i in range (0, len (event_list)):
                string += prefix + "TRACE_" + event_list[i].name() + ",\n"
            string += prefix + "TRACE_NB"
            return string.upper()

    def __parse_event_string_cb (self, event):
        """Create a default string interpretation for callbacks events."""
        assert event.callback
        string = "\"" + event.name ()
        for i in range (event.param_nb ()):
            string += " %d"
        string += "\\n\","
        return string

    def parse_event_string (self, event_list = None):
        """Parse the event list and construct a string containing on each line
        the string interpretation for a human."""
        if event_list != None:
            string = ""
            prefix = "    "
            for i in event_list:
                if i.callback:
                    string += prefix + self.__parse_event_string_cb (i)
                else:
                    string += prefix + i.string_get () + ",\n"
            return string

    def __read_template_header (self, string, string_nb):
        f = open (templatedir + '/template.h', 'r')
        template = f.read()
        f.close()
        template = template.replace('%%ids%%', string)
        template = template.replace('%%year%%', time.strftime ('%Y'))
        template = template.replace('%%NB%%', string_nb);
        return template

    def __read_template_file (self, string_table, string_nb):
        f = open (templatedir + '/template.c', 'r')
        template = f.read()
        f.close()
        template = template.replace('%%NB%%', string_nb);
        template = template.replace('%%DATA%%', string_table)
        return template

    def __write (self, template, outfile):
        f = open (outfile, 'w')
        f.write (template)
        f.close()

    def write_file (self, id_table, string_table, string_table_nb):
        template = self.__read_template_header (id_table, string_table_nb)
        self.__write (template, self.__header_name)
        template = self.__read_template_file (string_table, string_table_nb)
        self.__write (template, self.__file_name)

    def print_file (self, id_table, string_table, string_table_nb):
        template = self.__read_template_header (id_table, string_table_nb)
        print template
        template = self.__read_template_file (string_table, string_table_nb)
        print template
