from dfagen.automaton import *

%%

parser AutomatonParser:
    ignore:		"(#.*?)?\n"
    token EOF:		"$"
    token COMMENTS:	"  .+?\n"
    token STATE:	"\w+"
    token EVENT:	"\w([\w ]*\w)?"
    token QUALIFIER:	"\w([\w ]*\w)?"
    token ATITLE:	".*?\n"
    token ATTR:		"\w([\w =]*\w)?"
    token IMPORT:	"[\w./]+"

    rule automaton:	ATITLE		{{ a = Automaton (ATITLE.strip ()) }}
			( comments	{{ a.comments = comments }}
				) ?
			automatondef<<a>>
			EOF		{{ return a }}

    rule automatonsub<<a>>:
			ATITLE
			( comments ) ?
			automatondef<<a>>
			EOF

    rule automatondef<<a>>:
			( importdef<<a>> ) *
			"States:\n"
			( statedef	{{ a.add_state (statedef) }}
				) *
			( importdef<<a>> ) *
			"Events:\n"
			( eventdef	{{ a.add_event (eventdef) }}
				) *
			( importdef<<a>> ) *
			( transdef<<a>>
				) *

    rule statedef:			{{ initial = False }}
			" " ( "\*"	{{ initial = True }}
				) ?
			STATE		{{ s = State (STATE, initial = initial) }}
			( "\s*\[\s*"
				ATTR	{{ s.attributes = ATTR }}
				"\s*\]" ) ?
			"\n"
			( comments	{{ s.comments = comments }}
				) ?
					{{ return s }}

    rule eventdef:	" " EVENT	{{ e = Event (EVENT) }}
			"\n"
			( comments	{{ e.comments = comments }}
				) ?
					{{ return e }}

    rule transdef<<a>>:	transsl<<a>>
			( trans<<a>>	{{ for s in transsl: s.add_branch (trans) }}
				) *

    rule transsl<<a>>:			{{ sl = [ ] }}
			STATE		{{ sl.append (a.states[STATE]) }}
			( ",\s*" STATE	{{ sl.append (a.states[STATE]) }}
				) *
			":\n"		{{ return sl }}

    rule trans<<a>>:	" " EVENT	{{ t = TransitionBranch (a.events[EVENT]) }}
			( ":\s*" QUALIFIER {{ t.name = QUALIFIER }}
				) ?
			"\s*->\s*"
			( STATE		{{ t.to = a.states[STATE] }}
				| "\\." )
			( "\s*\[\s*"
				ATTR	{{ t.attributes = ATTR }}
				"\s*\]" ) ?
			( comments	{{ t.comments = comments }}
				) ?
					{{ return t }}

    rule comments:	COMMENTS	{{ c = COMMENTS.strip () }}
			( COMMENTS	{{ c += '\n' + COMMENTS.strip () }}
				) *	{{ return c }}

    rule importdef<<a>>:
			"import\s+"
			IMPORT		{{ import_automaton (IMPORT, a) }}
			"\n"

%%

def import_automaton (import_file, a):
    f = open (import_file, 'r')
    text = f.read ()
    f.close ()
    P = AutomatonParser (AutomatonParserScanner (text))
    return runtime.wrap_error_reporter (P, 'automatonsub', a)

