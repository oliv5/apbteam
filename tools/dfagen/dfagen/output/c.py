import os.path

import re

class StateData:
    """Data associated with a state when enumerating, used with templates."""

    def __init__ (self, state, prefix):
        self.state = state
        self.dict = dict (
                state = state.name,
                prefix = prefix,
                PREFIX = prefix.upper (),
                )

    def __getitem__ (self, key):
        # Second argument may be a default value.
        key = key.split ('|', 1) + [ '' ]
        key, default = key[0], key[1]
        val = None
        # Get value.
        if key in self.dict:
            val = self.dict[key]
        elif key == '@':
            val = self.state.attributes
        elif key.startswith ('@'):
            if self.state.attributes is not None:
                a = dict (p.split ('=')
                        for p in self.state.attributes.split ())
                try:
                    val = a[key[1:]]
                except KeyError:
                    pass
        else:
            raise KeyError, key
        # Test for empty value, and return.
        if val is None:
            val = default
        return val

class WriterData:

    callback_re = re.compile ('^\w+$')

    def __init__ (self, prefix, automaton, user):
        self.prefix = prefix
        self.automaton = automaton
        self.user = user
        self.states = self.automaton.iter_states
        self.initials = self.automaton.iter_initials
        self.events = self.automaton.iter_events
        self.dict = dict (
                prefix = prefix,
                PREFIX = prefix.upper (),
                name = automaton.name,
                comments = automaton.comments,
                states = self.list_states,
                initials = self.list_initials,
                events = self.list_events,
                states_names = self.list_states_names,
                events_names = self.list_events_names,
                initials_nb = str (len (automaton.initials)),
                branches = self.list_branches,
                only_branch_table = self.only_branch_table,
                transition_table = self.transition_table,
                states_template = self.states_template,
                )
        if len (automaton.initials) == 1:
            self.dict['initial'] = automaton.initials[0].name

    def list_states_sub (self, iter, template = None):
        if template is None:
            template = '%(PREFIX)s_STATE_%(state)s'
        return ''.join ('    '
                + template % StateData (s, self.prefix)
                + ',\n' for s in iter ())

    def list_states (self, template = None):
        return self.list_states_sub (self.states, template)

    def list_initials (self, template = None):
        return self.list_states_sub (self.initials, template)

    def list_events (self, template = None):
        if template is None:
            template = '%(PREFIX)s_EVENT_%(event)s'
        return ''.join ('    '
                + template % dict (
                    PREFIX = self.prefix.upper (),
                    event = e.name.replace (' ', '_')
                    )
                + ',\n' for e in self.events ())

    def list_states_names (self):
        return ''.join ('    "' + s.name + '",\n' for s in self.states ())

    def list_events_names (self):
        return ''.join ('    "' + e.name.replace (' ', '_') + '",\n'
            for e in self.events ())

    def list_branches (self):
        l = ''
        for s in self.states ():
            for tr in s.iter_transitions ():
                for br in tr.iter_branches ():
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

    def only_branch_table (self, template = None, null = None):
        if template is None:
            template = '%(PREFIX)s_STATE_%(state)s'
        if null is None:
            null = template
        r = ''
        for s in self.states ():
            r += '    { '
            es = [ ]
            for e in self.events ():
                to = None
                t = None
                if e in s.transitions and None in s.transitions[e].branches:
                    br = s.transitions[e].branches[None]
                    to = br.to and br.to.name or s.name
                    t = template
                else:
                    to = 'NB'
                    t = null
                es.append (t % dict (
                    PREFIX = self.prefix.upper (),
                    state = to,
                    ))
            r += ',\n      '.join (es)
            r += ' },\n'
        return r

    def transition_callback (self, state, event):
        attr = state.transitions[event].get_attributes ()
        if attr:
            assert len (attr) == 1, ("multiple callbacks for transition on "
                    + "event %s for state %s" % (event.name, state.name))
            callback = attr[0]
            assert self.callback_re.match (callback), ("bad callback name %s"
                    % callback)
            return callback
        else:
            return (self.prefix + '__' + state.name + '__'
                    + event.name.replace (' ', '_'))

    def transition_table (self):
        r = ''
        for s in self.states ():
            r += '    { '
            es = [ ]
            for e in self.events ():
                if e in s.transitions:
                    es.append (self.transition_callback (s, e))
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
        for s in self.states ():
            for tr in s.iter_transitions ():
                d = WriterData (self.prefix, self.automaton, self.user)
                branches_to = '\n'.join (
                        [(br.name and br.name or '')
                            + ' => '
                            + (br.to and br.to.name or s.name)
                            + (br.comments and ('\n  '
                                + br.comments.replace ('\n', '\n  ')) or '')
                            for br in tr.iter_branches ()])
                returns = '\n'.join (
                        ['    return ' + self.prefix + '_next'
                            + (br.name and '_branch' or '')
                            + ' (' + s.name + ', '
                            + tr.event.name.replace (' ', '_')
                            + (br.name and ', ' + br.name.replace (' ', '_')
                                or '')
                            + ');'
                            for br in tr.iter_branches ()])
                d.dict = dict (
                        prefix = self.prefix,
                        user = self.user,
                        state = s.name,
                        event = tr.event.name.replace (' ', '_'),
                        branches_to = branches_to,
                        returns = returns,
                        callback = self.transition_callback (s, tr.event),
                        )
                exp += tt % d
        return exp

    def __getitem__ (self, key):
        preproc = lambda v: v
        args = []
        key = key.split (',')
        key, args = key[0], key[1:]
        if key.startswith ('*'):
            key = key[1:]
            preproc = lambda v: ' * ' + v.replace ('\n', '\n * ') + '\n'
        if key.startswith ('_'):
            key = key[1:]
            preproc = lambda v: v and v + '\n' or ''
        val = None
        if key in self.dict:
            try:
                val = self.dict[key] (*args)
            except TypeError:
                val = self.dict[key]
        elif key.startswith ('user.'):
            val = self.user[key[5:]]
        val = preproc (val)
        if val is None:
            raise KeyError, key
        return val

class Writer:

    def __init__ (self, data, templatedir, outputdir):
        data.templatedir = templatedir
        self.data = data
        self.templatedir = templatedir
        self.outputdir = outputdir

    def write_template (self, template, output):
        t = open (os.path.join (self.templatedir, template), 'r')
        tt = t.read ()
        t.close ()
        exp = tt % self.data
        o = open (os.path.join (self.outputdir, output), 'w')
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

def write (prefix, automaton, user, outputdir, origin):
    if origin is not None:
        raise NotImplementedError ("--origin is not implemented for C output")
    templatedir = os.path.splitext (__file__)[0]
    if 'template-dir' in user:
        templatedir = os.path.join (os.path.split (user.file)[0],
                user['template-dir'])
    w = Writer (WriterData (prefix, automaton, user), templatedir, outputdir)
    w.write ()

