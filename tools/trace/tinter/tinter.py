import sys
from thost import *
from utils import *

try:
    from lib.parser import *
except:
    print "--> You should run yapps on lib/parser.g"


class TInter:

    def __init__(self, infile, outfile):
        self.__infile = infile
        self.__events = None
        self.__outfile = outfile
        self.__file = None

    def __events_get (self):
        infile = open (self.__infile, 'r')
        events = parse ('parser', infile.read())
        infile.close()
        return events

    def __event_print (self, events, memory):
        if len(memory) > 0:
            cmd = get_size (memory, 2)
            if cmd < len(events):
                e = events[cmd]
                string = e.string_get()
                for i in range (0, e.param_nb()):
                    p = e.param_get(i)
                    size = p.length()
                    val = get_size (memory, size)
                    string = string.replace('%d', str(val), 1)

                if self.__file == None:
                    print string[1:len(string)-1]
                else:
                    self.__file.write (string[1:len(string)-1] + "\n")

                return memory
            else:
                return None

    def trace_print (self):
        events = self.__events_get ()
        host = THost()
        host.dump_memory()
        memory = host.get_trace ()

        if self.__outfile != None:
            self.__file = open (self.__outfile, 'w')
            self.__file.write ('APBTeam v1.0 Log interpretor\n')

        while memory != None:
            memory = self.__event_print(events, memory)

        if self.__file != None:
            self.__file.close ()
