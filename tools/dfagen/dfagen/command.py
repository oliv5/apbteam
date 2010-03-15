import dfagen.output
try:
    import dfagen.parser
except ImportError:
    print "--> You should run yapps on parser.g!"
    raise

from optparse import OptionParser

def run ():
    opt = OptionParser ()
    opt.add_option ('-d', '--dfa', dest='dfa',
            help='read DFA description from FILE', metavar='FILE')
    opt.add_option ('-o', '--output', dest='output',
            help='choose output format', metavar='OUTPUT')
    opt.add_option ('-c', '--config', dest='config',
            help='read output configuration from FILE', metavar='FILE')
    opt.add_option ('-p', '--prefix', dest='prefix',
            help='use PREFIX for generated output', metavar='PREFIX')
    opt.add_option ('--origin', dest='origin', action='append',
            help='restrict output to given automaton name(s)', metavar='NAME')
    opt.add_option ('-O', '--output-dir', dest='output_dir', default='',
            help='generate output in DIR', metavar='DIR')
    opt.add_option ('--dump', action='store_true', default=False,
            help='dump the read automaton')
    (options, args) = opt.parse_args ()
    if (options.dfa is None
            or not options.dump and options.output is None
            or len (args) != 0):
        opt.error ('bad arguments')
    # Read automaton.
    f = open (options.dfa, 'r')
    a = dfagen.parser.parse ('automaton', f.read ())
    f.close ()
    # Dump automaton.
    if options.dump:
        print a
    # Read config.
    cfg = dfagen.output.UserConfig (options.config)
    # Produce output.
    if options.output is not None:
        dfagen.output.get_output (options.output).write (options.prefix, a,
                cfg, options.output_dir, options.origin)
