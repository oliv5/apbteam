import os.path

def write (prefix, automaton, user, outputdir):
    output = prefix + '.dot'
    o = open (os.path.join (outputdir, output), 'w')
    o.write ('digraph %s {' % prefix)
    for s in automaton.iter_states ():
        o.write (' %s\n' % s.name)
        for tr in s.iter_transitions ():
            for br in tr.iter_branches ():
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
