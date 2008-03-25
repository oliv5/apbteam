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
    # TODO add more error checking.
    (options, args) = opt.parse_args ()
    # Read automaton.
    f = open (options.dfa, 'r')
    a = dfagen.parser.parse ('automaton', f.read ())
    f.close ()
    # Read config.
    cfg = dfagen.output.UserConfig (options.config)
    # Produce output.
    dfagen.output.get_output (options.output).write (options.prefix, a, cfg)
