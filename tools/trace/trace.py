#!/bin/usr/pythonenv

import sys
from tcreator.tcreator import *
from tinter.tinter import *

def create_parse_args(list = None):
        infile = ""
        outfile = ""
        enum_name = ""
        if list == None:
                return None
        else:
                for i in range (0, len(list)):
                        if list[i] == "-e":
                                enum_name = list[i+1]
                        if list[i] == "-o":
                                outfile = list[i+1]
                        else:
                                infile = list[i]
        return [infile, outfile, enum_name]


print "Trace System v1.0 by APBTeam\n"

try:
        if len(sys.argv) > 1:
                argc = len(sys.argv)
                if sys.argv[1] == "create":
                        if (argc >= 2) and (argc <= 7):
                                data = create_parse_args(sys.argv)
                                cre = TCreator (data[0], data[1], data[2])
                                cre.create ()
                        else:
                                raise Exception ("Not enough arguments")
                if sys.argv[1] == "inter":
                        if argc == 3:
                                inter = TInter (sys.argv[2])
                                inter.trace_print()
                        else:
                                raise Exception ("Not enough arguments")
        else:
                raise Exception ("Not enough arguments")
except:
        print "Trace system use..."
        print "python trace.py create [options] <file in>"
        print "  Options : "
        print "     -e enum name"
        print "     -o file out name"
        print ""
        print "python trace.py inter <file in>"
        print ""
