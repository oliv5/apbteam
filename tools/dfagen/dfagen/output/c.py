import os.path

class WriterData:

    def __init__ (self, prefix, automaton, user):
	self.prefix = prefix
	self.automaton = automaton
	self.user = user
	self.states = self.automaton.states.values ()
	self.events = self.automaton.events.values ()
	self.dict = dict (
		prefix = prefix,
		PREFIX = prefix.upper (),
		name = automaton.name,
		comments = automaton.comments,
		initial = automaton.initial.name,
		states = self.list_states,
		events = self.list_events,
		branches = self.list_branches,
		transition_table = self.transition_table,
		cb_impl = self.cb_impl,
		cb_decl = self.cb_decl,
		)

    def list_states (self):
	return ''.join (['    ' + self.prefix.upper () + '_STATE_' + s.name
	    + ',\n' for s in self.states])

    def list_events (self):
	return ''.join (['    ' + self.prefix.upper () + '_EVENT_'
	    + e.name.replace (' ', '_') + ',\n' for e in self.events])

    def list_branches (self):
	l = ''
	for s in self.states:
	    for tr in s.transitions.values ():
		for br in tr.branches.values ():
		    n = dict (
			    PREFIX = self.prefix.upper (),
			    state = s.name,
			    event = tr.event.name.replace (' ', '_'),
			    branch = (br.name and br.name.replace (' ', '_')
				or ''),
			    to = (br.to and br.to.name or s.name),
			    )
		    l += ('    %(PREFIX)s_BRANCH__%(state)s__%(event)s__%(branch)s = '
			    + '_BRANCH (%(state)s, %(event)s, %(to)s),\n') % n
	return l

    def transition_table (self):
	r = ''
	for s in self.states:
	    r += '    { '
	    es = [ ]
	    for e in self.events:
		if e in s.transitions:
		    es.append (self.prefix + '__' + s.name + '__'
			    + e.name.replace (' ', '_'))
		else:
		    es.append ('NULL')
	    r += ',\n      '.join (es)
	    r += ' },\n'
	return r

    def states_template (self, template):
	t = open (os.path.join (self.templatedir, template), 'r')
	tt = t.read ()
	t.close ()
	exp = ''
	for s in self.states:
	    for tr in s.transitions.values ():
		d = WriterData (self.prefix, self.automaton, self.user)
		branches_to = '\n'.join (
			[(br.name and br.name or '')
			    + ' => '
			    + (br.to and br.to.name or s.name)
			    + (br.comments and ('\n  '
				+ br.comments.replace ('\n', '\n  ')) or '')
			    for br in tr.branches.values ()])
		returns = '\n'.join (
			['    return ' + self.prefix + '_next'
			    + (br.name and '_branch' or '')
			    + ' (' + s.name + ', '
			    + tr.event.name.replace (' ', '_')
			    + (br.name and ', ' + br.name.replace (' ', '_')
				or '')
			    + ');'
			    for br in tr.branches.values ()])
		d.dict = dict (
			prefix = self.prefix,
			user = self.user,
			state = s.name,
			event = tr.event.name.replace (' ', '_'),
			branches_to = branches_to,
			returns = returns,
			)
		exp += tt % d
	return exp

    def cb_impl (self):
	return self.states_template ('template_cb_impl.c')

    def cb_decl (self):
	return self.states_template ('template_cb_decl.h')

    def __getitem__ (self, key):
	preproc = lambda v: v
	if key.startswith ('*'):
	    key = key[1:]
	    preproc = lambda v: ' * ' + v.replace ('\n', '\n * ') + '\n'
	if key.startswith ('_'):
	    key = key[1:]
	    preproc = lambda v: v and v + '\n' or ''
	val = None
	if key in self.dict:
	    try:
		val = self.dict[key] ()
	    except TypeError:
		val = self.dict[key]
	elif key.startswith ('user.'):
	    val = self.user[key[5:]]
	val = preproc (val)
	if val is None:
	    raise KeyError, key
	return val

class Writer:

    def __init__ (self, data, templatedir):
	data.templatedir = templatedir
	self.data = data
	self.templatedir = templatedir

    def write_template (self, template, output):
	t = open (os.path.join (self.templatedir, template), 'r')
	tt = t.read ()
	t.close ()
	exp = tt % self.data
	o = open (output, 'w')
	o.write (exp)
	o.close ()

    def write (self):
	templates = self.data.user.templates
	if not templates:
	    templates = {
		    'template.h': '%.h',
		    'template.c': '%.c',
		    'template_cb.h': '%_cb.h',
		    'template_cb_skel.c': '%_cb_skel.c',
		    }
	for (t, f) in templates.iteritems ():
	    self.write_template (t, f.replace ('%', self.data.prefix))

def write (prefix, automaton, user):
    w = Writer (WriterData (prefix, automaton, user), 'template-dir' in user
	    and user['template-dir'] or os.path.splitext (__file__)[0])
    w.write ()

