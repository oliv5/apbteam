import sys
from writer import *

try:
    from lib.parser import *
except:
    print "--> You should run yapps on lib/parser.g"

class TCreator:
    def __init__(self, infile):
        self.__infile = infile

    def create (self, outfile):
        infile = open (self.__infile, 'r')
        data = parse ('parser', infile.read())
        infile.close()
        w = Writer ()
        id_table = w.parse_event_identifiers (data)
        string_table = w.parse_event_string (data)
        string_table_nb = str (len (data))
        if outfile:
            w.write_file (id_table, string_table, string_table_nb)
        else:
            w.print_file (id_table, string_table, string_table_nb)
