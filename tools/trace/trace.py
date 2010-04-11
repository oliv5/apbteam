#!/bin/usr/env python

from optparse import OptionParser
import sys
from tcreator.tcreator import *
from tinter.tinter import *

INTER_MODE=" (only available on interpretor  mode)."
CREATE_MODE=" (only available on create mode)."

print "Trace System by APBTeam\n"

usage = "usage: %prog [options] [host program]"
parser = OptionParser(usage=usage)
parser.add_option("-o", "--output", dest="outfile", action="store_true",
        help="generate events.h and events.c files" + CREATE_MODE)
parser.add_option("-i", "--infile", dest="infile",
        help="Read the data from the file")
parser.add_option("-t", "--type", dest="type",
        help="create to create the enumeration, inter to read the log from the flash.")
parser.add_option("-l", "--list", dest="list_trace", action="store_true",
        help="List the number of the traces" + INTER_MODE)
parser.add_option("-n", "--num", type="int", dest="trace",
        help="Dump the trace number provided" + INTER_MODE)
parser.add_option("-c", "--callback", dest="cb",
        help="The callback file containing the functions" + INTER_MODE)

[options, args] = parser.parse_args()
if options.type == "inter":
    assert args

if options.type == 'create':
    cre = TCreator (options.infile)
    cre.create (options.outfile)
elif options.type == "inter":
    inter = TInter (options.infile, args[0], options.cb)
    if options.list_trace:
        inter.available_traces ()
    elif options.trace != None:
        inter.trace_print (options.trace)
    else:
        inter.trace_print()
else:
    parser.print_help ()
