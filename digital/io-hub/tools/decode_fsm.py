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
    fsm_re = re.compile(r'!F([0-9a-f]{2})([0-9a-f]{2})([0-9a-f]{2})')
    robot_re = re.compile(r'!R([0-9a-f]{4})([0-9a-f]{4})')
    obstacle_re = re.compile(r'!O([0-9a-f]{4})([0-9a-f]{4})')
    for l in sys.stdin:
        for m in fsm_re.finditer(l):
            old, event, new = [ int(i, 16) for i in m.groups() ]
            try:
                old = states[old]
                event = events[event]
                new = states[new]
            except KeyError:
                print "unknown transition"
            else:
                print "%s -> %s -> %s" % (old, event, new)
        for m in robot_re.finditer(l):
            x, y = [ int(i, 16) for i in m.groups() ]
            print "robot %s %s" % (x, y)
        for m in obstacle_re.finditer(l):
            x, y = [ int(i, 16) for i in m.groups() ]
            print "obstacle %s %s" % (x, y)

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
