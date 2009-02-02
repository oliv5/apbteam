import sys
from writer import *

try:
    from lib.parser import *
except:
    print "--> You should run yapps on lib/parser.g"

class TCreator:
        
    def __init__(self, infile, outfile, enum_name = "trace_id_t"):
        self.__infile = infile
        self.__outfile = outfile
        self.__enum_name = enum_name

    def create (self):
        infile = open (self.__infile, 'r')
        data = parse ('parser', infile.read())
        infile.close()

        w = Writer (self.__outfile, self.__enum_name)
        outstring = w.parse_list (data)

        if self.__outfile != "":
            w.write_file (outstring)
        else:
            w.print_file (outstring)
