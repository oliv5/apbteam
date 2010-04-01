def filter_origin_iter (list, origin):
    if origin is None:
        return iter (list)
    else:
        import itertools
        return itertools.ifilter (lambda x: x.origin in origin, iter (list))

class Event:
    """Event definition."""

    def __init__ (self, name, comments = '', origin = None):
        self.name = name
        self.comments = comments
        self.origin = origin
        pass

    def __str__ (self):
        s = ' ' + self.name + '\n'
        if self.comments:
            s += '  ' + self.comments.replace ('\n', '\n  ') + '\n'
        return s

class State:
    """State definition."""

    def __init__ (self, name, comments = '', initial = False, origin = None):
        self.name = name
        self.comments = comments
        self.initial = initial
        self.origin = origin
        self.transitions = { }
        self.transitions_list = [ ]
        self.attributes = None

    def __str__ (self):
        s = ' ' + self.name
        if self.attributes:
            s += ' [ %s ]' % self.attributes
        s += '\n'
        if self.comments:
            s += '  ' + self.comments.replace ('\n', '\n  ') + '\n'
        return s

    def add_branch (self, branch):
        if branch.event not in self.transitions:
            tr = Transition (branch.event)
            self.transitions[branch.event] = tr
            self.transitions_list.append (tr)
        self.transitions[branch.event].add_branch (branch)

    def iter_transitions (self):
        return iter (self.transitions_list)

class Transition:
    """Transition definition."""

    def __init__ (self, event):
        self.event = event
        self.branches = { }
        self.branches_list = [ ]

    def add_branch (self, branch):
        assert self.event is branch.event
        if branch.name == None and self.branches:
            raise KeyError (branch.name)
        if branch.name != None and None in self.branches:
            raise KeyError (branch.name)
        if branch.name in self.branches:
            raise KeyError (branch.name)
        self.branches[branch.name] = branch
        self.branches_list.append (branch)

    def iter_branches (self, origin = None):
        return filter_origin_iter (self.branches_list, origin)

    def get_attributes (self):
        return [ b.attributes for b in self.iter_branches ()
                if b.attributes is not None ]

    def __str__ (self):
        return ''.join (str (br) for br in self.iter_branches ())

class TransitionBranch:

    def __init__ (self, event, name = None, to = None, comments = '',
            origin = None):
        self.event = event
        self.name = name
        self.to = to
        self.comments = comments
        self.origin = origin
        self.attributes = None

    def __str__ (self):
        s = ' ' + self.event.name
        if self.name:
            s += ': ' + self.name
        s += ' -> ' + (self.to and self.to.name or '.')
        if self.attributes:
            s += ' [ %s ]' % self.attributes
        s += '\n'
        if self.comments:
            s += '  ' + self.comments.replace ('\n', '\n  ') + '\n'
        return s

class Automaton:

    def __init__ (self, name):
        self.name = name
        self.comments = ''
        self.initials = [ ]
        self.states = { }
        self.states_list = [ ]
        self.events = { }
        self.events_list = [ ]

    def add_state (self, state):
        if state.name in self.states:
            raise KeyError (state.name)
        if not self.states:
            state.initial = True
        if state.initial:
            self.initials.append (state)
        self.states[state.name] = state
        self.states_list.append (state)

    def add_event (self, event):
        if event.name in self.events:
            raise KeyError (event.name)
        self.events[event.name] = event
        self.events_list.append (event)

    def iter_states (self, origin = None):
        return filter_origin_iter (self.states_list, origin)

    def iter_initials (self, origin = None):
        return filter_origin_iter (self.initials, origin)

    def iter_events (self, origin = None):
        return filter_origin_iter (self.events_list, origin)

    def __str__ (self):
        s = self.name + '\n'
        if self.comments:
            s += '  ' + self.comments.replace ('\n', '\n  ') + '\n'
        s += '\nStates:\n'
        s += ''.join (str (state) for state in self.iter_states ())
        s += '\nEvents:\n'
        s += ''.join (str (event) for event in self.iter_events ())
        s += '\n'
        for state in self.iter_states ():
            s += state.name + ':\n'
            s += ''.join (str (tr) for tr in state.iter_transitions ())
            s += '\n'
        return s
