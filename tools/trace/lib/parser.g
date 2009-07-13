import sys

from traceclass import *

%%

parser TraceParser:
    ignore:         "(#.*?)?\n"
    token START:    "^"
    token EOF:      "$"
    token EVENT:    "[a-zA-Z_]+"
    token PARAM:    "[a-zA-Z_1-9]+"
    token LENGTH:   "[1-2-4]"
    token STRING:   "\".*\""
    token CB:	    "[a-zA-Z_]+"
    token SPACE:    " "

    rule parser: START      {{ my_list = list() }}
                 (event     {{ my_list.append (event) }}
                        )*
                 EOF        {{ return my_list }}

    rule event:         "EVENT " EVENT          {{ e = TraceEvent(EVENT.strip()) }}
                        SPACE
                        ( param SPACE           {{ e.param_add (param[0], param[1]) }}
                                )*
                        ( string                {{ e.string_set (string) }}
                                )?
			( cb			{{ e.callback = cb }}
				)?
                        "\n"                    {{ return e }}

    rule param: PARAM SPACE LENGTH              {{ return [PARAM.strip(), int(LENGTH.strip())] }}

    rule string: STRING                         {{ return (STRING.strip()) }}

    rule cb: "\[" CB "\]" 			{{ return (CB.strip()) }}
