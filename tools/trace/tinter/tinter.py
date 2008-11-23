import sys
from thost import *

try:
        from lib.parser import *
except:
        print "--> You should run yapps on lib/parser.g"

class TInter:

        def __init__(self, infile):
                self.__infile = infile
                self.__events = None

        def __events_get (self):
                infile = open (self.__infile, 'r')
                events = parse ('parser', infile.read())
                infile.close()
                return events

        def __event_print (self, events, memory):
                if len(memory) > 0:
                        cmd = int (memory[0:2])
                        e = events[cmd]
                        string = e.string_get()
                        memory = memory[2:len(memory)]
                        for i in range (0, e.param_nb()):
                                p = e.param_get(i)
                                size = p.length() * 2
                                val = memory[0:size]
                                memory = memory[size:len(memory)]
                                string = string.replace('%d', str(int(val, 16)), 1) 
                        return [memory, string]

        def trace_print (self):
                events = self.__events_get ()
                memory = thost_dump_memory()

                while len(memory) > 0:
                        data = self.__event_print(events, memory)
                        string = data[1]
                        memory = data[0]
                        print string[1:len(string)-1]
