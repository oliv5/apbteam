#!/bin/usr/env python

from optparse import OptionParser
import sys
sys.path.append ('../../host/')
from tcreator.tcreator import *
from tinter.tinter import *

print "Trace System v1.0 by APBTeam\n"

parser = OptionParser()
parser.add_option("-e", "--enum-name", dest="enum_name",
        help="provide the enumerator name", metavar="ENUM")
parser.add_option("-o", "--output", dest="outfile",
        help="Store the eunerator on the output file")
parser.add_option("-i", "--infile", dest="infile",
        help="Read the data from the file")
parser.add_option("-t", "--type", dest="type",
        help="create to create the enumeration, inter to read the log from the flash.")

[options, args] = parser.parse_args()

if options.type == 'create':
    cre = TCreator (options.infile, options.outfile, options.enum_name)
    cre.create ()
elif options.type == "inter":
    inter = TInter (options.infile, options.outfile)
    inter.trace_print()
else:
    print "see the help\n python trace --help"
