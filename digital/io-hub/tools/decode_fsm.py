#!/usr/bin/python
"""This script will read FSM header file and proto output and decode to human
readable transitions."""
import re
import sys
import optparse

def parse_header(fsm_name, fname):
    """Parse header file and return a dict of states and a dict of events."""
    r = dict(state = { }, event = { })
    state_re = re.compile(r'^\s*FSM_(STATE)_%s_(.*) = (\d+),$' % fsm_name)
    event_re = re.compile(r'^\s*FSM_(EVENT)_%s_(.*) = (\d+),$' % fsm_name)
    with open(fname) as f:
        for l in f:
            m = state_re.match(l) or event_re.match(l)
            if m:
                t, name, value = m.groups()
                t = t.lower()
                value = int(value)
                r[t][value] = name
    return r['state'], r['event']

def parse_proto(states, events):
    """Parse proto output from stdin and output FSM transitions."""
    fsm_re = re.compile(r'^!F([0-9af]{2})([0-9af]{2})([0-9af]{2})$')
    for l in sys.stdin:
        m = fsm_re.match(l)
        if m:
            old, event, new = [ int(i, 16) for i in m.groups() ]
            try:
                old = states[old]
                event = events[event]
                new = states[new]
            except 1:
                print "unknown transition"
            else:
                print "%s -> %s -> %s" % (old, event, new)

op = optparse.OptionParser(description=__doc__)
op.add_option('--file', '-f', metavar='HEADER',
        help="give path to generated header file (default: %default)",
        default='fsm_AI_gen.h')
op.add_option('--fsm-name', '-n', metavar='NAME',
        help="FSM name (default: %default)",
        default='AI')
options, args = op.parse_args()
if args:
    op.error("too many arguments")

states, events = parse_header(options.fsm_name, options.file)
parse_proto(states, events)
