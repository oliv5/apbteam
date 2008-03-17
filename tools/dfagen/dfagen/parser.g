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

    rule automaton:	ATITLE		{{ a = Automaton (ATITLE.strip ()) }}
			( comments	{{ a.comments = comments }}
				) ?
			"States:\n"
			( statedef	{{ a.add_state (statedef) }}
				) *
			"Events:\n"
			( eventdef	{{ a.add_event (eventdef) }}
				) *
			( transdef<<a>>
				) *
			EOF		{{ return a }}
    
    rule statedef:	" " STATE	{{ s = State (STATE) }}
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
			( comments	{{ t.comments = comments }}
				) ?
					{{ return t }}

    rule comments:	COMMENTS	{{ c = COMMENTS.strip () }}
			( COMMENTS	{{ c += '\n' + COMMENTS.strip () }}
				) *	{{ return c }}

