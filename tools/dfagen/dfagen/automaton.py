class Event:
    """Event definition."""

    def __init__ (self, name, comments = ''):
	self.name = name
	self.comments = comments
	pass

    def __str__ (self):
	s = ' ' + self.name + '\n'
	if self.comments:
	    s += '  ' + self.comments.replace ('\n', '\n  ') + '\n'
	return s

class State:
    """State definition."""

    def __init__ (self, name, comments = ''):
	self.name = name
	self.comments = comments
	self.transitions = { }

    def __str__ (self):
	s = ' ' + self.name + '\n'
	if self.comments:
	    s += '  ' + self.comments.replace ('\n', '\n  ') + '\n'
	return s

    def add_branch (self, branch):
	if branch.event not in self.transitions:
	    self.transitions[branch.event] = Transition (branch.event)
	self.transitions[branch.event].add_branch (branch)

class Transition:
    """Transition definition."""

    def __init__ (self, event):
	self.event = event
	self.branches = { }

    def add_branch (self, branch):
	assert self.event is branch.event
	if branch.name == None and self.branches:
	    raise KeyError (branch.name)
	if branch.name != None and None in self.branches:
	    raise KeyError (branch.name)
	if branch.name in self.branches:
	    raise KeyError (branch.name)
	self.branches[branch.name] = branch

    def __str__ (self):
	s = ''
	for br in self.branches.values ():
	    s += str (br);
	return s

class TransitionBranch:

    def __init__ (self, event, name = None, to = None, comments = ''):
	self.event = event
	self.name = name
	self.to = to
	self.comments = comments

    def __str__ (self):
	s = ' ' + self.event.name
	if self.name:
	    s += ': ' + self.name
	s += ' -> ' + (self.to and self.to.name or '.') + '\n'
	if self.comments:
	    s += '  ' + self.comments.replace ('\n', '\n  ') + '\n'
	return s

class Automaton:
    
    def __init__ (self, name):
	self.name = name
	self.comments = ''
	self.initial = None
	self.states = { }
	self.events = { }

    def add_state (self, state):
	if state.name in self.states:
	    raise KeyError (state.name)
	if not self.states:
	    self.initial = state
	self.states[state.name] = state

    def add_event (self, event):
	if event.name in self.events:
	    raise KeyError (event.name)
	self.events[event.name] = event

    def __str__ (self):
	s = self.name
	if self.comments:
	    s += '  ' + self.comments.replace ('\n', '\n  ') + '\n'
	s += '\nStates:\n'
	for state in self.states.values ():
	    s += str (state)
	s += '\nEvents:\n'
	for event in self.events.values ():
	    s += str (event)
	s += '\n'
	for state in self.states.values ():
	    s += state.name + ':\n'
	    for tr in state.transitions.values ():
		s += str (tr)
	    s += '\n'
	return s
