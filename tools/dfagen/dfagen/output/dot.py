
def write (prefix, automaton, user):
    output = prefix + '.dot'
    o = open (output, 'w')
    o.write ('digraph %s {' % prefix)
    for s in automaton.states.values ():
	o.write (' %s\n' % s.name)
	for tr in s.transitions.values ():
	    for br in tr.branches.values ():
		o.write (' %(state)s -> %(to)s [ label = "%(event)s" ];\n'
			% dict (
			    state = s.name,
			    event = tr.event.name
			    + (br.name and ': ' + br.name or ''),
			    to = (br.to and br.to.name or s.name),
			    )
			)
    o.write ('}\n')
    o.close ()
