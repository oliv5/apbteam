#!/usr/bin/python
"""This script will read FSM header file, connect to target and decode to
human readable transitions."""
import re
import sys
import optparse

import io_hub
from utils.init_proto import InitProto

def parse_header(fsm_name, fname):
    """Parse header file and return dict of states, events and branches."""
    r = dict(state = { }, event = { }, branch = { })
    r['branch'][255] = None
    state_re = re.compile(r'^\s*angfsm_(STATE)_%s_(.*) = (\d+),$' % fsm_name)
    event_re = re.compile(r'^\s*angfsm_(EVENT)_%s_(.*) = (\d+),$' % fsm_name)
    branch_re = re.compile(r'^\s*angfsm_(BRANCH)_%s_(.*) = (\d+),$' % fsm_name)
    with open(fname) as f:
        for l in f:
            m = state_re.match(l) or event_re.match(l) or branch_re.match(l)
            if m:
                t, name, value = m.groups()
                t = t.lower()
                value = int(value)
                r[t][value] = name
    return r['state'], r['event'], r['branch']

ip = InitProto(None, io_hub.Proto)
ip.parser.add_option('--file', '-f', metavar='HEADER',
        help="give path to generated header file (default: %default)",
        default='angfsm_gen_arm_AI.h')
ip.parser.add_option('--fsm-name', '-n', metavar='NAME',
        help="FSM name (default: %default)",
        default='AI')
ip.parse_args()
states, events, branches = parse_header(ip.options.fsm_name, ip.options.file)

io = ip.get_proto()

def transition(state, event, output_state, branch):
    try:
        state = states[state]
        event = events[event]
        output_state = states[output_state]
        branch = branches[branch]
    except KeyError:
        print "unknown transition"
    else:
        if branch is None:
            print "%s -> %s -> %s" % (state, event, output_state)
        else:
            print "%s -> %s -> %s (%s)" % (state, event, branch, output_state)

io.register_transitions(transition)
try:
    io.proto.wait()
except KeyboardInterrupt:
    pass
