#!/bin/usr/env python

from optparse import OptionParser
import sys
sys.path.append ('../../host/')
from tcreator.tcreator import *
from tinter.tinter import *

INTER_MODE=" (only available on interpretor  mode)."
CREATE_MODE=" (only available on create mode)."

print "Trace System v1.0 by APBTeam\n"

parser = OptionParser()
parser.add_option("-e", "--enum-name", dest="enum_name",
        help="provide the enumerator name" + CREATE_MODE,
        metavar="ENUM")
parser.add_option("-o", "--output", dest="outfile",
        help="Store the enumerator on the output file" + CREATE_MODE)
parser.add_option("-i", "--infile", dest="infile",
        help="Read the data from the file")
parser.add_option("-t", "--type", dest="type",
        help="create to create the enumeration, inter to read the log from the flash.")
parser.add_option("-l", "--list", dest="list_trace", action="store_true",
        help="List the number of the traces" + INTER_MODE)
parser.add_option("-n", "--num", type="int", dest="trace",
        help="Dump the trace number provided" + INTER_MODE)

[options, args] = parser.parse_args()

if options.type == 'create':
    cre = TCreator (options.infile, options.outfile, options.enum_name)
    cre.create ()
elif options.type == "inter":
    inter = TInter (options.infile, options.outfile)
    if options.list_trace:
        inter.available_traces ()
    elif options.trace:
        inter.trace_print (options.trace)
    else:
        inter.trace_print()
else:
    print "see the help\n python trace --help"
