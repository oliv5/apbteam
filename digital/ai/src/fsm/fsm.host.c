/*
   AngFSM - AngFSM - Almost Non Generated Finite State Machine

   Copyright 2011 Jerome Jutteau

Contact:
 * email: j.jutteau _AT_ gmail _DOT_ com
 * website: http://fuu.im/angfsm/

 This file is part of AngFSM.

 AngFSM is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 AngFSM is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with AngFSM. If not, see <http://www.gnu.org/licenses/>.
 */

#include "fsm.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
fsm_build_arg_parse (char *string, char ***tab, int *nb)
{
    long i = 0;
    long buff_cnt = 0;
    char *buff;
    int nb_cur = 0;
    int param = 0;

    assert (string && tab && nb);
    if (strlen (string) == 0)
      {
	*nb = 0;
	**tab = NULL;
	return;
      }
    buff = strdup (string);

    /* Count. */
    *nb = 1;
    for (i = 0; string[i] != '\0'; i++)
	if (string[i] == ',')
	    (*nb)++;
    *tab = (char **) malloc ((*nb) * sizeof (char *));

    /* Fill. */
    for (i = 0; i < (long) strlen (string) + 1; i++)
      {
	if (string[i] == ',' || string[i] == '\0')
	  {
	    param = 0;
	    buff[buff_cnt] = '\0';
	    (*tab)[nb_cur] = strdup (buff);
	    buff_cnt=0;
	    nb_cur++;
	  }
	else if (string[i] != '"' && string[i] != ' ')
	  {
	    if (param == 0)
		param = 1;
	    if (param == 2)
		printf ("Parse error in \"%s\"\n", string);
	    buff[buff_cnt++] = string[i];
	  }
	else if (string[i] == ' ')
	  {
	    if (param == 1)
		param = 2;
	  }
      }
    free (buff);
}

void
fsm_build_arg_free (char ***tab, int nb)
{
    assert (*tab);
    int i;
    for (i = 0; i < nb; i++)
      {
	assert ((*tab)[i]);
	free ((*tab)[i]);
      }
    free (*tab);
}

void
fsm_build_print (fsm_build_t *fsm,
		 fsm_build_trans_t *trans,
		 fsm_build_branch_chain_t *branch)
{
    //XXX What to do here ?
    if (branch->name == NULL)
	fprintf (stderr, "Transition: %s -- %s --> %s\n",
		 trans->state->var_name,
		 trans->event->var_name,
		 branch->state->var_name);
    else
	fprintf (stderr, "Transition: %s -- %s --> %s (%s)\n",
		 trans->state->var_name,
		 trans->event->var_name,
		 branch->name,
		 branch->state->var_name);
}

void
fsm_build_sanity_check (fsm_build_t *fsm)
{
    fsm_build_event_chain_t *ec;
    fsm_build_event_chain_t *ec_tmp;
    fsm_build_state_chain_t *sc;
    fsm_build_state_chain_t *sc_tmp;
    fsm_build_trans_chain_t *tc;
    fsm_build_trans_chain_t *tc_tmp;
    fsm_build_branch_chain_t *bc;
    uint b;

    /* Sanity check 1: are all states has a different name ? */
    sc = fsm->states;
    while (sc != NULL)
      {
	sc_tmp = fsm->states;
	while (sc_tmp != NULL)
	  {
	    if (sc != sc_tmp
		&& strcmp (sc->state.var_name, sc_tmp->state.var_name)
		== 0)
		assert (!"All states must have a different name.");
	    sc_tmp = sc_tmp->next;
	  }
	sc = sc->next;
      }

    /* Sanity check 2: are all events has a different name ? */
    ec = fsm->events;
    while (ec != NULL)
      {
	ec_tmp = fsm->events;
	while (ec_tmp != NULL)
	  {
	    if (ec != ec_tmp
		&& strcmp (ec->event.var_name, ec_tmp->event.var_name)
		== 0)
		assert (!"All events must have a different name.");
	    ec_tmp = ec_tmp->next;
	  }
	ec = ec->next;
      }

    /* Sanity check 3: are all events used in a transition ? */
    ec = fsm->events;
    while (ec != NULL)
      {
	b = 0;
	tc = fsm->trans;
	while (tc != NULL)
	  {
	    if (&ec->event == tc->trans.event)
	      {
		b = 1;
		break;
	      }
	    tc = tc->next;
	  }
	if (!b)
	    fprintf (stderr, "Warning, some events are not used in a transition (%s).\n",
		     ec->event.var_name);
	ec = ec->next;
      }

    /* Sanity check 4: are all states used in a transition ? */
    sc = fsm->states;
    while (sc != NULL)
      {
	b = 0;
	tc = fsm->trans;
	while (tc != NULL)
	  {
	    if (&sc->state == tc->trans.state)
	      {
		b = 1;
		break;
	      }
	    /* This state may be a final state. */
	    bc = tc->trans.output_branches;
	    while (bc != NULL)
	      {
		if (&sc->state == bc->state)
		  {
		    b = 1;
		    break;
		  }
		bc = bc->next;
	      }

	    tc = tc->next;
	  }
	if (!b)
	    fprintf (stderr, "Warning, some states are not used in a transition (%s).\n",
		     sc->state.var_name);
	sc = sc->next;
      }

    /* Sanity check 5: are all states can be reached during execution ? */
    fsm_build_state_chain_t *stack = NULL;
    fsm_build_state_chain_t *tail = NULL;
    uint i;
    /* Initialise check array. */
    uint *check_tab = malloc (fsm->state_nb * sizeof (uint));
    memset (check_tab, 0, fsm->state_nb * sizeof (uint));
    /* Initialize by adding first active states the stack. */
    sc = fsm->starters;
    while (sc != NULL)
      {
	sc_tmp = (fsm_build_state_chain_t *)
	    malloc (sizeof (fsm_build_state_chain_t));
	sc_tmp->state = sc->state;
	if (stack == NULL)
	    stack = sc_tmp;
	if (tail != NULL)
	    tail->next = sc_tmp;
	sc_tmp->next = NULL;
	tail = sc_tmp;
	sc = sc->next;
      }
    /* Get next states from the stack and check them. */
    while (stack != NULL)
      {
	/* Check state. */
	check_tab[stack->state.code] = 1;
	/* Add to the tail of the stack th next states if they are not checked
	 * yet. */
	tc = fsm->trans;
	while (tc != NULL)
	  {
	    if (tc->trans.state->code == stack->state.code)
	      {
		bc = tc->trans.output_branches;
		while (bc != NULL)
		  {
		    if (!check_tab[bc->state->code])
		      {
			check_tab[bc->state->code] = 1;
			/* Add at the end of the stack. */
			sc_tmp = (fsm_build_state_chain_t *)
			    malloc (sizeof (fsm_build_state_chain_t));
			sc_tmp->state = *bc->state;
			sc_tmp->next = NULL;
			tail->next = sc_tmp;
			tail = sc_tmp;
		      }
		    bc = bc->next;
		  }
	      }
	    tc = tc->next;
	  }
	sc_tmp = stack->next;
	free (stack);
	stack = sc_tmp;
      }
    /* Check if some states are not reached. */
    for (i = 0; i < fsm->state_nb; i++)
	if (!check_tab[i])
	    fprintf (stderr, "Warning, some states won't be reachable during execution (%s).\n",
		     fsm_build_get_state_by_code (fsm, i)->var_name);
    /* Free. */
    free (check_tab);

    /* Sanity check 6: Any transition how have the same state and same event ?
     * Useless test ? Making the same state/event couple will cause a
     * compilation error.
     */
    tc = fsm->trans;
    while (tc != NULL)
      {
	tc_tmp = fsm->trans;
	while (tc_tmp != NULL)
	  {
	    if (tc != tc_tmp
		&& tc->trans.state == tc_tmp->trans.state
		&& tc->trans.event == tc_tmp->trans.event)
		assert (!"All transitions must have a different state/event\
			couple.");
	    tc_tmp = tc_tmp->next;
	  }
	tc = tc->next;
      }

    /* Sanity check 7: check that all first active states are unique. */
    sc = fsm->starters;
    while (sc != NULL)
      {
	uint b = 0;
	sc_tmp = fsm->starters;
	while (sc_tmp != NULL)
	  {
	    if (sc_tmp->state.code == sc->state.code)
		b++;
	    if (b > 1)
		assert (!"Your first active states must be unique.");
	    sc_tmp = sc_tmp->next;
	  }
	sc = sc->next;
      }

    /* Sanity check 8: any transition output error ?
     * for example, as we are in state s42, making a FSM_NEXT (s1, e2) will
     * work but this is a user's mistake.
     *
     * TODO Find a way to check this.
     **/
}

void
fsm_build_reset (fsm_build_t *fsm)
{
    uint i;
    fsm_build_state_chain_t *curs = fsm->starters;
    for (i = 0; i < fsm->max_active_states; i++)
      {
	fsm->run.active_states[i] = NULL;
	fsm->run.events_before_active_state[i] = NULL;
      }
    i = 0;
    while (curs != NULL)
      {
	fsm->run.active_states[i++] = &curs->state;
	curs = curs->next;
      }
}

void
fsm_build_gen_dot (fsm_build_t *fsm)
{
    static uint cpt = 1;
    uint j;
    fsm_build_trans_chain_t *tc;
    fsm_build_branch_chain_t *bc;

    uint fns = strlen (fsm->name) + strlen ("dot__.dot") + cpt + 1;
    char *fn = malloc (fns * sizeof (char));
    snprintf (fn, fns, "dot_%s_%u.dot", fsm->name, cpt++);
    FILE *f = fopen(fn,"w");

    /* Gen graph. */
    fprintf (f, "digraph %s {\n", fsm->name);
    tc = fsm->trans;
    while (tc != NULL)
      {
	bc = tc->trans.output_branches;
	while (bc != NULL)
	  {
	    fprintf (f, "\t%s -> %s [label=\"%s",
		     tc->trans.state->var_name,
		     bc->state->var_name,
		     tc->trans.event->var_name);
	    if (bc->name != NULL)
		fprintf (f, " (%s)", bc->name);
	    fprintf (f, "\"");
	    /* If the next state is an active state we look if this this event
	     * was handled before. */
	    for (j = 0; j < fsm->max_active_states; j++)
		if (fsm->run.active_states[j]
		    && fsm->run.active_states[j]->code == bc->state->code)
		    if (fsm->run.events_before_active_state[j]
			&& fsm->run.events_before_active_state[j]->code
			== tc->trans.event->code)
			fprintf (f, ", color=red");

	    fprintf (f, "];\n");
	    bc = bc->next;
	  }
	tc = tc->next;
      }
    /* Colorise active states. */
    for (j = 0; j < fsm->max_active_states; j++)
	if (fsm->run.active_states[j])
	    fprintf (f, "\t%s [color=blue, fontcolor=white, style=filled];\n",
		     fsm->run.active_states[j]->var_name);

    fprintf (f, "}\n\n");

    /* Close file. */
    fclose (f);

    /* Free. */
    free (fn);
}

/* All fsm. */
fsm_build_chain_t *fsm_build_all_fsm;
void fsm_build_init_all_fsm ()
{
    fsm_build_all_fsm = NULL;
}

/* Just initialise structures. */
void
fsm_build_init (fsm_build_t *fsm, char *name)
{
    assert (fsm);
    fsm->events = NULL;
    fsm->states = NULL;
    fsm->trans = NULL;
    fsm->name = name;
    fsm->max_active_states = 0;
    fsm->event_nb = 0;
    fsm->state_nb = 0;
    fsm->starters = NULL;
    fsm->timeouts = NULL;

    fsm->run.trans_table = NULL;
    fsm->run.active_states = NULL;
    fsm->run.events_before_active_state = NULL;
    fsm->run.func_pool = NULL;
    fsm->run.timeout_counters = NULL;

    /* Store fsm. */
    fsm_build_chain_t *niou = (fsm_build_chain_t *)
	malloc (sizeof (fsm_build_chain_t));
    niou->fsm = fsm;
    niou->next = fsm_build_all_fsm;
    fsm_build_all_fsm = niou;
}

/* Prepare the fsm to run (at very end). */
void
fsm_build_run_init (fsm_build_t *fsm)
{
    uint i,j;

    /* Sanity check. */
    fsm_build_sanity_check (fsm);

    /* Create empty transition table. */
    fsm->run.trans_table = (fsm_build_run_strans_func_t **)
	malloc (fsm->event_nb * sizeof (fsm_build_run_strans_func_t *));
    for (i = 0; i < fsm->event_nb; i++)
      {
	fsm->run.trans_table[i] = (fsm_build_run_strans_func_t *)
	    malloc (fsm->state_nb * sizeof (fsm_build_run_strans_func_t));
	for (j = 0; j < fsm->state_nb; j++)
	    fsm->run.trans_table[i][j] = NULL;
      }

    /* Fill with transitions. */
    fsm_trans_func_chain_t *fc = fsm->run.func_pool;
    while (fc != NULL)
      {
	fsm->run.trans_table[fc->trans.event->code][fc->trans.state->code] =
	    fc->func;
	fc = fc->next;
      }

    /* Free function pool. */
    fsm_trans_func_chain_t *old_fc;
    fc = fsm->run.func_pool;
    while (fc != NULL)
      {
	old_fc = fc;
	fc = fc->next;
	free (old_fc);
      }
    fsm->run.func_pool = NULL;

    /* Initialize active states. */
    fsm->run.active_states = (fsm_build_state_t **) malloc
	(fsm->max_active_states * sizeof (fsm_build_state_t *));
    for (i = 0; i < fsm->max_active_states; i++)
	fsm->run.active_states[i] = NULL;
    fsm_build_state_chain_t *s = fsm->starters;
    i = 0;
    while (s != NULL)
      {
	fsm->run.active_states[i++] = &s->state;
	s = s->next;
      }

    /* Initialize last events before active states. */
    fsm->run.events_before_active_state = (fsm_build_event_t **) malloc
	(fsm->max_active_states * sizeof (fsm_build_event_t *));
    for (i = 0; i < fsm->max_active_states; i++)
	fsm->run.events_before_active_state[i] = NULL;

    /* Initialize timeout counters. */
    fsm->run.timeout_counters = (int *)
	malloc (fsm->max_active_states * sizeof (int));
    for (i = 0; i < fsm->max_active_states; i++)
	fsm->run.timeout_counters[i] = -1;
    fsm_build_timeout_chain_t *toc = fsm->timeouts;
    for (i = 0; i < fsm->max_active_states; i++)
	while (toc != NULL)
	  {
	    if (fsm->run.active_states[i]->code
		== toc->timeout.trans->state->code)
		fsm->run.timeout_counters[i] = toc->timeout.timeout;
	    toc = toc->next;
	  }
}

void
fsm_build_states (fsm_build_t *fsm, char *states)
{
    fsm_build_state_chain_t *head = fsm->states;
    char **args;
    int i, nb;

    fsm_build_arg_parse (states, &args, &nb);
    for (i = 0; i < nb; i++)
      {
	fsm_build_state_chain_t *s = (fsm_build_state_chain_t *)
	    malloc (sizeof (fsm_build_state_chain_t));
	s->state.var_name = strdup (args[i]);
	s->state.code = fsm->state_nb++;
	s->next = head;
	head = s;
      }
    fsm_build_arg_free (&args, nb);
    fsm->states = head;
}

void
fsm_build_events (fsm_build_t *fsm, char *events)
{
    fsm_build_event_chain_t *head = fsm->events;
    char **args;
    int i, nb;

    if (strcmp (events, "_TIMEOUT_") == 0
	&& fsm_build_get_event (fsm, events) != NULL)
	return;
    fsm_build_arg_parse (events, &args, &nb);
    for (i = 0; i < nb; i++)
      {
	fsm_build_event_chain_t *e = (fsm_build_event_chain_t *)
	    malloc (sizeof (fsm_build_event_chain_t));
	e->event.var_name = strdup (args[i]);
	e->event.code = fsm->event_nb++;
	e->next = head;
	head = e;
      }
    fsm_build_arg_free (&args, nb);
    fsm->events = head;
}

fsm_build_event_t *
fsm_build_get_event (fsm_build_t *fsm, char *event)
{
    fsm_build_event_chain_t *curs = fsm->events;
    while (curs != NULL)
      {
	if (strcmp (curs->event.var_name, event) == 0)
	    return &(curs->event);
	curs = curs->next;
      }
    return NULL;
}

fsm_build_state_t *
fsm_build_get_state (fsm_build_t *fsm, char *state)
{
    fsm_build_state_chain_t *curs = fsm->states;
    while (curs != NULL)
      {
	if (strcmp (curs->state.var_name, state) == 0)
	    return &(curs->state);
	curs = curs->next;
      }
    return NULL;
}

fsm_build_event_t*
fsm_build_get_event_by_code (fsm_build_t *fsm, uint event)
{
    fsm_build_event_chain_t  *curs = fsm->events;
    while (curs != NULL)
      {
	if (curs->event.code == event)
	    return &(curs->event);
	curs = curs->next;
      }
    return NULL;
}

fsm_build_state_t*
fsm_build_get_state_by_code (fsm_build_t *fsm, uint state)
{
    fsm_build_state_chain_t *curs = fsm->states;
    while (curs != NULL)
      {
	if (curs->state.code == state)
	    return &(curs->state);
	curs = curs->next;
      }
    return NULL;
}

uint16_t
fsm_build_get_event_code (fsm_build_t *fsm, char *event)
{
    assert (fsm);
    fsm_build_event_t *e = fsm_build_get_event (fsm, event);
    assert (e);
    return e->code;
}

void
fsm_build_trans (fsm_build_t *fsm,
		 char *state,
		 char *event,
		 char *output_branches,
		 fsm_build_run_strans_func_t trans_func)
{
    fsm_build_trans_t t;
    fsm_build_branch_chain_t *b;
    char **args;
    int i, nb;

    t.output_branches = NULL;
    /* Check state and event exists. */
    t.state = fsm_build_get_state (fsm, state);
    t.event = fsm_build_get_event (fsm, event);
    assert (t.state);
    assert (t.event);

    fsm_build_arg_parse (output_branches, &args, &nb);
    /* Only one output state. */
    if (nb == 1)
      {
	t.output_branches = (fsm_build_branch_chain_t *)
	    malloc (sizeof (fsm_build_branch_chain_t));
	t.output_branches->name = NULL;
	t.output_branches->state = fsm_build_get_state (fsm, args[0]);
	assert (t.output_branches->state);
	t.output_branches->next = NULL;
      }
    else
	for (i = 0; i < nb; i = i + 2)
	  {
	    b = (fsm_build_branch_chain_t *)
		malloc (sizeof (fsm_build_branch_chain_t));
	    b->name = strdup (args[i]);
	    b->state = fsm_build_get_state (fsm, args[i+1]);
	    assert (b->state);
	    b->next = t.output_branches;
	    t.output_branches = b;
	  }
    fsm_build_arg_free (&args, nb);

    /* Add trans to fsm. */
    fsm_build_trans_chain_t *tc = (fsm_build_trans_chain_t *)
	malloc (sizeof (fsm_build_trans_chain_t));
    tc->trans = t;
    tc->next = fsm->trans;
    fsm->trans = tc;

    /* Add trans function to run context. */
    fsm_trans_func_chain_t *fc = (fsm_trans_func_chain_t *)
	malloc (sizeof (fsm_trans_func_chain_t));
    fc->func = trans_func;
    fc->trans = tc->trans;
    fc->next = fsm->run.func_pool;
    fsm->run.func_pool = fc;
}

void
fsm_build_timeout (fsm_build_t *fsm, char *state, char *event, uint timeout)
{
    fsm_build_trans_chain_t *tc = NULL;
    fsm_build_trans_t *t = NULL;
    fsm_build_state_t *s = NULL;
    fsm_build_event_t *e = NULL;
    fsm_build_timeout_chain_t *to = NULL;

    assert (fsm);
    assert (state);
    assert (event);

    /* Find the corresponding transition. */
    tc = fsm->trans;
    s = fsm_build_get_state (fsm, state);
    e = fsm_build_get_event (fsm, event);
    assert (tc);
    assert (s);
    assert (e);
    while (tc != NULL)
      {
	if (tc->trans.state == s && tc->trans.event == e)
	  {
	    t = &tc->trans;
	    break;
	  }
	tc = tc->next;
      }
    assert (t);

    /* Fill a new timeout. */
    to = (fsm_build_timeout_chain_t *)
	malloc (sizeof (fsm_build_timeout_chain_t));
    to->timeout.timeout = timeout;
    to->timeout.trans = t;

    /* Add timeout to chain. */
    to->next = fsm->timeouts;
    fsm->timeouts = to;
}

void
fsm_build_start_with (fsm_build_t *fsm, char *starters)
{
    char **args;
    int nb, i;
    fsm_build_arg_parse (starters, &args, &nb);
    fsm->max_active_states += nb;
    for (i=0; i < nb; i++)
      {
	fsm_build_state_chain_t *sc = (fsm_build_state_chain_t *)
	    malloc (sizeof (fsm_build_state_chain_t));
	fsm_build_state_t *s = fsm_build_get_state (fsm, args[i]);
	assert (s);
	sc->state = *s;
	sc->next = fsm->starters;
	fsm->starters = sc;
      }
    fsm_build_arg_free (&args, nb);
}

int
fsm_build_handle (fsm_build_t *fsm, fsm_build_event_t *e)
{
    fsm_build_state_t *s = NULL;
    fsm_build_timeout_chain_t *toc = NULL;
    assert (e);
    uint i;
    int handled = 0;
    for (i = 0; i < fsm->max_active_states; i++)
      {
	s = fsm->run.active_states[i];
	if (s && fsm->run.trans_table[e->code][s->code])
	  {
	    fsm->run.events_before_active_state[i] = e;
	    fsm->run.active_states[i] = fsm->run.trans_table[e->code][s->code]();
	    /* Check the new state has a timeout or not. */
	    toc = fsm->timeouts;
	    fsm->run.timeout_counters[i] = -1;
	    while (toc != NULL)
	      {
		if (toc->timeout.trans->state->code
		    == fsm->run.active_states[i]->code)
		  {
		    fsm->run.timeout_counters[i] = toc->timeout.timeout;
		    break;
		  }
		toc = toc->next;
	      }

	    handled = 1;
	  }
      }
    return handled;
}

int
fsm_build_handle_string (fsm_build_t *fsm, char *event)
{
    fsm_build_event_t *e = fsm_build_get_event (fsm, event);
    return fsm_build_handle (fsm, e);
}

int
fsm_build_handle_integer (fsm_build_t *fsm, uint16_t event)
{
    fsm_build_event_t *e = fsm_build_get_event_by_code (fsm, event);
    return fsm_build_handle (fsm, e);
}

int
fsm_build_can_handle (fsm_build_t *fsm, fsm_build_event_t *e)
{
    fsm_build_state_t *s = NULL;
    assert (e);
    uint i;
    for (i = 0; i < fsm->max_active_states; i++)
      {
	s = fsm->run.active_states[i];
	if (s && fsm->run.trans_table[e->code][s->code])
	    return 1;
      }
    return 0;
}

int
fsm_build_can_handle_string (fsm_build_t *fsm, char *event)
{
    fsm_build_event_t *e = fsm_build_get_event (fsm, event);
    return fsm_build_can_handle (fsm, e);
}

int
fsm_build_can_handle_integer (fsm_build_t *fsm, uint16_t event)
{
    fsm_build_event_t *e = fsm_build_get_event_by_code (fsm, event);
    return fsm_build_can_handle (fsm, e);
}

int
fsm_build_handle_timeout (fsm_build_t *fsm)
{
    int out = 0;
    int i;
    char *event = NULL;
    for (i = 0; i < (int) fsm->max_active_states; i++)
      {
	if (fsm->run.timeout_counters[i] > 0)
	    fsm->run.timeout_counters[i]--;
	/* We have a timeout event. */
	if (fsm->run.timeout_counters[i] == 0)
	  {
	    fsm->run.timeout_counters[i] = -1;
	    /* build event string */
	    event = (char *) malloc (
				     (strlen (fsm->run.active_states[i]->var_name)
				      + strlen ("_TIMEOUT") + 1) * sizeof (char));
	    sprintf (event, "%s_TIMEOUT", fsm->run.active_states[i]->var_name);
	    fsm_build_handle_string (fsm, event);
	    out = 1;
	  }
      }
    return out;
}

fsm_build_state_t*
fsm_build_get_next_state (fsm_build_t *fsm,
			  char *state,
			  char *event,
			  char *branch)
{
    fsm_build_state_t *s;
    fsm_build_event_t *e;
    fsm_build_trans_chain_t *t_curs;
    fsm_build_branch_chain_t *b_curs;

    /* Convert input data. */
    s = fsm_build_get_state (fsm, state);
    e = fsm_build_get_event (fsm, event);
    assert (s && e);

    /* Get transition. */
    t_curs = fsm->trans;
    while (t_curs != NULL)
      {
	if (s == t_curs->trans.state && e == t_curs->trans.event)
	    break;
	t_curs = t_curs->next;
      }
    assert (t_curs);
    assert (t_curs->trans.output_branches);

    /* If we have only one branch. */
    if (strlen (branch) == 0)
      {
	/* Branch has to be given is there are multiple branches. */
	assert (t_curs->trans.output_branches->next == NULL);
	fsm_build_print (fsm, &t_curs->trans, t_curs->trans.output_branches);
	return t_curs->trans.output_branches->state;
      }

    /* Find correct branch.  */
    b_curs = t_curs->trans.output_branches;
    while (b_curs != NULL)
      {
	if (strcmp (b_curs->name, branch) == 0)
	    break;
	b_curs = b_curs->next;
      }
    assert (b_curs);
    fsm_build_print (fsm, &t_curs->trans, b_curs);
    return b_curs->state;
}

void
fsm_build_gen_avr_h (fsm_build_t *fsm, uint embedded_strings)
{
    fsm_build_state_chain_t *sc;
    fsm_build_event_chain_t *ec;
    fsm_build_trans_chain_t *tc;
    fsm_build_branch_chain_t *bc;
    fsm_build_state_t *s;
    fsm_build_event_t *e;
    fsm_build_chain_t *all_fsm;
    uint i, j;

    /* Open file. */
    char *fn = (char *) malloc ((strlen (fsm->name)
				 + strlen ("fsm_gen_.h") + 1) * sizeof (char));
    sprintf (fn, "fsm_%s_gen.h", fsm->name);
    FILE *f = fopen (fn, "w");

    /* Introduction. */
    fprintf (f, "/* This file has been generated, do not edit. */\n\n");
    fprintf (f, "#ifndef _FSM_%s_\n", fsm->name);
    fprintf (f, "#define _FSM_%s_\n\n", fsm->name);
    fprintf (f, "#include <avr/pgmspace.h>\n");
    fprintf (f, "#include <inttypes.h>\n\n");

    /* Include all other fsm headers. */
    all_fsm = fsm_build_all_fsm;
    while (all_fsm != NULL)
      {
	fprintf (f, "#include \"fsm_%s_gen.h\"\n",all_fsm->fsm->name);
	all_fsm = all_fsm->next;
      }

    /* Gen max active states define */
    fprintf (f, "#define fsm_%s_max_active_states %u\n",
	     fsm->name,
	     fsm->max_active_states);

    /* Gen state enum. */
    fprintf (f, "typedef enum\n{\n");
    sc = fsm->states;
    while (sc != NULL)
      {
	fprintf (f, "\tFSM_STATE_%s_%s = %u,\n", fsm->name, sc->state.var_name, sc->state.code);
	sc = sc->next;
      }
    fprintf (f, "\tFSM_STATE_%s_NB_ = %u\n", fsm->name, fsm->state_nb);
    fprintf (f, "} fsm_%s_state_t;\n\n", fsm->name);

    /* Gen event enum. */
    fprintf (f, "typedef enum\n{\n");
    ec = fsm->events;
    while (ec != NULL)
      {
	fprintf (f, "\tFSM_EVENT_%s_%s = %u,\n", fsm->name, ec->event.var_name, ec->event.code);
	ec = ec->next;
      }
    fprintf (f, "\tFSM_EVENT_%s_NB_ = %u\n", fsm->name, fsm->event_nb);
    fprintf (f, "} fsm_%s_event_t;\n\n", fsm->name);

    /* Gen state strings. */
    if (embedded_strings)
      {
	sc = fsm->states;
	while (sc != NULL)
	  {
	    fprintf (f, "extern prog_char fsm_%s_state_str_%s[%u] PROGMEM;\n",
		     fsm->name,
		     sc->state.var_name,
		     strlen (sc->state.var_name) + 1);
	    sc = sc->next;
	  }
	fprintf (f, "extern const char *fsm_%s_state_str[%u] PROGMEM;\n\n",
		 fsm->name,
		 fsm->state_nb);

	/* Gen event strings. */
	ec = fsm->events;
	while (ec != NULL)
	  {
	    fprintf (f, "extern prog_char fsm_%s_event_str_%s[%u] PROGMEM;\n",
		     fsm->name,
		     ec->event.var_name,
		     strlen (ec->event.var_name) + 1);
	    ec = ec->next;
	  }
	fprintf (f, "extern const char *fsm_%s_event_str[%u] PROGMEM;\n\n",
		 fsm->name,
		 fsm->event_nb);

	/* Create a RAM string able to store event or state string. */
	j = 0;
	for (i = 0; i < fsm->event_nb; i++)
	  {
	    e = fsm_build_get_event_by_code (fsm, i);
	    if (strlen (e->var_name) > j)
		j = strlen (e->var_name);
	  }
	for (i = 0; i < fsm->state_nb; i++)
	  {
	    s = fsm_build_get_state_by_code (fsm, i);
	    if (strlen (s->var_name) > j)
		j = strlen (s->var_name);
	  }
	fprintf (f, "extern char fsm_%s_str_buff[%u];\n", fsm->name, j + 1);

	/* Convert an event enum in string. */
	fprintf (f, "char *\nfsm_%s_get_event_string_from_enum \
		 (fsm_%s_event_t e);\n", fsm->name, fsm->name);

	/* Convert a event string in enum. */
	fprintf (f, "fsm_%s_event_t\nfsm_%s_get_event_enum_from_string \
		 (char *str);\n", fsm->name, fsm->name);

	/* Convert an state enum in string. */
	fprintf (f, "char *\nfsm_%s_get_state_string_from_enum \
		 (fsm_%s_state_t s);\n",	fsm->name, fsm->name);

	/* Convert a state string in enum. */
	fprintf (f, "fsm_%s_state_t\nfsm_%s_get_state_enum_from_string \
		 (char *str);\n", fsm->name, fsm->name);
      }

    /* Gen transitions branches enum. */
    fprintf (f, "typedef enum\n{\n");
    tc = fsm->trans;
    while (tc != NULL)
      {
	bc = tc->trans.output_branches;
	while (bc != NULL)
	  {
	    if (bc->name != NULL)
		fprintf (f, "\tFSM_BRANCH_%s_%s_%s_%s = %u,\n",
			 fsm->name,
			 tc->trans.state->var_name,
			 tc->trans.event->var_name,
			 bc->name,
			 bc->state->code);
	    else
		fprintf (f, "\tFSM_BRANCH_%s_%s_%s_ = %u,\n",
			 fsm->name,
			 tc->trans.state->var_name,
			 tc->trans.event->var_name,
			 bc->state->code);
	    bc = bc->next;
	  }
	tc = tc->next;
      }
    fprintf (f, "} fsm_%s_branch_t;\n\n", fsm->name);

    /* Gen function headers. */
    tc = fsm->trans;
    while (tc != NULL)
      {
	fprintf (f, "fsm_%s_branch_t fsm_%s_trans_func_%s_%s (void);\n",
		 fsm->name,
		 fsm->name,
		 tc->trans.state->var_name,
		 tc->trans.event->var_name);
	tc = tc->next;
      }
    fprintf (f, "\n");

    /* Gen function table. */
    fprintf (f, "typedef fsm_%s_branch_t (*fsm_%s_func_t)(void);\n", fsm->name,
	     fsm->name);
    fprintf (f, "extern const fsm_%s_func_t PROGMEM fsm_%s_trans_table[%u][%u];\n",
	     fsm->name,
	     fsm->name,
	     fsm->event_nb,
	     fsm->state_nb);
    /* Gen read function for trans table. */
    fprintf (f, "fsm_%s_func_t fsm_%s_read_trans (fsm_%s_event_t event, "
             "fsm_%s_state_t state);\n\n",
             fsm->name,
             fsm->name,
             fsm->name,
             fsm->name);

    /* Gen active states array. */
    fprintf (f, "extern fsm_%s_state_t fsm_%s_active_states[%u];\n\n",
	     fsm->name,
	     fsm->name,
	     fsm->max_active_states);

    /* Gen initialization function. */
    sc = fsm->starters;
    i = 0;
    fprintf (f, "void\nfsm_%s_init () __attribute__ ((constructor));\n\n",
	     fsm->name);

    /* Gen handle function. */
    fprintf (f, "int\nfsm_%s_handle (fsm_%s_event_t e);\n",
	     fsm->name,
	     fsm->name);

    /* Gen can handle function. */
    fprintf (f, "uint16_t\nfsm_%s_can_handle (fsm_%s_event_t e);\n\n",
	     fsm->name,
	     fsm->name);

    if (fsm->timeouts != NULL)
      {
	/* Gen handle timeout function. */
	fprintf (f, "int\nfsm_%s_handle_timeout ();\n",
		 fsm->name);

	/* Gen timeout values. */
	fprintf (f, "extern int32_t fsm_%s_timeout_values[FSM_STATE_%s_NB_];\n",
		 fsm->name,
		 fsm->name);

	/* Gen timeout corresponding events. */
	fprintf (f, "extern fsm_%s_event_t fsm_%s_timeout_events[FSM_STATE_%s_NB_];\n",
		 fsm->name,
		 fsm->name,
		 fsm->name);

	/* Gen timeout counters array. */
	fprintf (f, "extern int32_t fsm_%s_timeout_counters[%u];\n\n",
		 fsm->name,
		 fsm->max_active_states);
      }

    /* Conclusion. */
    fprintf (f, "#endif /* #ifndef _FSM_%s_ */", fsm->name),

	    /* Close file. */
	    fclose (f);

    /* Free. */
    free (fn);
}

void
fsm_build_gen_avr_c (fsm_build_t *fsm, uint embedded_strings)
{
    fsm_build_state_chain_t *sc;
    fsm_build_event_chain_t *ec;
    fsm_build_trans_chain_t *tc;
    fsm_build_timeout_chain_t *toc;
    fsm_build_state_t *s;
    fsm_build_event_t *e;
    uint i, j, found;

    /* Open file. */
    char *fn = (char*) malloc ((strlen (fsm->name)
				+ strlen ("fsm_gen_.c") + 1) * sizeof (char));
    sprintf (fn, "fsm_%s_gen.c", fsm->name);
    FILE *f = fopen (fn, "w");

    /* Introduction. */
    fprintf (f, "/* This file has been generated, do not edit. */\n\n");
    fprintf (f, "#include \"fsm_%s_gen.h\"\n\n", fsm->name);
    fprintf (f, "#include \"modules/proto/proto.h\"\n\n");

    /* Gen state strings. */
    if (embedded_strings)
      {
	sc = fsm->states;
	while (sc != NULL)
	  {
	    fprintf (f, "prog_char fsm_%s_state_str_%s[] PROGMEM = \"%s\";\n",
		     fsm->name,
		     sc->state.var_name,
		     sc->state.var_name);
	    sc = sc->next;
	  }
	fprintf (f, "const char *fsm_%s_state_str[] PROGMEM =\n{\n", fsm->name);
	for (i = 0; i < fsm->state_nb; i++)
	  {
	    s = fsm_build_get_state_by_code (fsm, i);
	    fprintf (f, "\tfsm_%s_state_str_%s", fsm->name, s->var_name);
	    if (i == fsm->state_nb - 1)
		fprintf (f, "\n");
	    else
		fprintf (f, ",\n");
	  }
	fprintf (f, "};\n\n");

	/* Gen event strings. */
	ec = fsm->events;
	while (ec != NULL)
	  {
	    fprintf (f, "prog_char fsm_%s_event_str_%s[] PROGMEM = \"%s\";\n",
		     fsm->name,
		     ec->event.var_name,
		     ec->event.var_name);
	    ec = ec->next;
	  }
	fprintf (f, "const char *fsm_%s_event_str[] PROGMEM =\n{\n", fsm->name);
	for (i = 0; i < fsm->event_nb; i++)
	  {
	    e = fsm_build_get_event_by_code (fsm, i);
	    fprintf (f, "\tfsm_%s_event_str_%s", fsm->name, e->var_name);
	    if (i == fsm->event_nb - 1)
		fprintf (f, "\n");
	    else
		fprintf (f, ",\n");
	  }
	fprintf (f, "};\n\n");

	/* Create a RAM string able to store event or state string. */
	j = 0;
	for (i = 0; i < fsm->event_nb; i++)
	  {
	    e = fsm_build_get_event_by_code (fsm, i);
	    if (strlen (e->var_name) > j)
		j = strlen (e->var_name);
	  }
	for (i = 0; i < fsm->state_nb; i++)
	  {
	    s = fsm_build_get_state_by_code (fsm, i);
	    if (strlen (s->var_name) > j)
		j = strlen (s->var_name);
	  }
	fprintf (f, "char fsm_%s_str_buff[%u];\n", fsm->name, j + 1);

	/* Convert an event enum in string. */
	fprintf (f, "char *\nfsm_%s_get_event_string_from_enum \
		 (fsm_%s_event_t e)\n{\n", fsm->name, fsm->name);
	fprintf (f, "\treturn strcpy_P (fsm_%s_str_buff, \
	    (char *) pgm_read_word (&(fsm_%s_event_str[e])));\n", fsm->name, fsm->name);
	fprintf (f, "}\n\n");

	/* Convert a event string in enum. */
	fprintf (f, "fsm_%s_event_t\nfsm_%s_get_event_enum_from_string \
		 (char *str)\n{\n", fsm->name, fsm->name);
	fprintf (f, "\tuint16_t i;\n");
	fprintf (f, "\tfor (i = 0; i < FSM_EVENT_%s_NB_; i++)\n", fsm->name);
	fprintf (f, "\t\tif (strcpy_P (str, \
	    (char *) pgm_read_word (&(fsm_%s_event_str[i]))) == 0)\n", fsm->name);
	fprintf (f, "\t\t\treturn i;\n");
	fprintf (f, "\treturn FSM_EVENT_%s_NB_;\n", fsm->name);
	fprintf (f, "}\n\n");

	/* Convert an state enum in string. */
	fprintf (f, "char *\nfsm_%s_get_state_string_from_enum \
		 (fsm_%s_state_t s)\n{\n", fsm->name, fsm->name);
	fprintf (f, "\treturn strcpy_P (fsm_%s_str_buff, \
	    (char *) pgm_read_word (&(fsm_%s_state_str[s])));\n", fsm->name, fsm->name);
	fprintf (f, "}\n\n");

	/* Convert a state string in enum. */
	fprintf (f, "fsm_%s_state_t\nfsm_%s_get_state_enum_from_string \
		 (char *str)\n{\n", fsm->name, fsm->name);
	fprintf (f, "\tuint16_t i;\n");
	fprintf (f, "\tfor (i = 0; i < FSM_STATE_%s_NB_; i++)\n", fsm->name);
	fprintf (f, "\t\tif (strcpy_P (str, \
	    (char *) pgm_read_word (&(fsm_%s_state_str[i]))) == 0)\n", fsm->name);
	fprintf (f, "\t\t\treturn i;\n");
	fprintf (f, "\treturn FSM_STATE_%s_NB_;\n", fsm->name);
	fprintf (f, "}\n\n");
      }

    /* Gen function table. */
    fprintf (f, "const fsm_%s_func_t PROGMEM fsm_%s_trans_table[%u][%u] = \n{\n",
	     fsm->name,
	     fsm->name,
	     fsm->event_nb,
	     fsm->state_nb);
    /* for each events and state, see if it exists an associated transition. */
    for (i = 0; i < fsm->event_nb; i++)
      {
	e = fsm_build_get_event_by_code (fsm, i);
	fprintf (f, "\t{");
	for (j = 0; j < fsm->state_nb; j++)
	  {
	    s = fsm_build_get_state_by_code (fsm, j);
	    tc = fsm->trans;
	    found = 0;
	    while (tc != NULL)
	      {
		if (tc->trans.state == s && tc->trans.event == e)
		  {
		    found = 1;
		    fprintf (f, "&fsm_%s_trans_func_%s_%s",
			     fsm->name,
			     tc->trans.state->var_name,
			     tc->trans.event->var_name);
		    tc = tc->next;
		    break;
		  }
		tc = tc->next;
	      }
	    if (!found)
		fprintf (f, "(fsm_%s_func_t) 0", fsm->name);
	    if (j == fsm->state_nb - 1)
		fprintf (f, "}");
	    else
		fprintf (f, ", ");
	  }
	if (i != fsm->event_nb - 1)
	    fprintf (f, ",");
	fprintf (f, "\n");
      }
    fprintf (f, "};\n\n");

    /* Gen read function for trans table. */
    fprintf (f, "fsm_%s_func_t fsm_%s_read_trans (fsm_%s_event_t event, "
             "fsm_%s_state_t state)\n{\n",
             fsm->name,
             fsm->name,
             fsm->name,
             fsm->name);
    fprintf (f, "\treturn (fsm_%s_func_t) pgm_read_word "
             "(&fsm_%s_trans_table[event][state]);\n",
             fsm->name,
             fsm->name);
    fprintf (f, "}\n\n");

    /* Gen active states array. */
    fprintf (f, "fsm_%s_state_t fsm_%s_active_states[%u];\n\n",
	     fsm->name,
	     fsm->name,
	     fsm->max_active_states);

    /* Gen initialization function. */
    sc = fsm->starters;
    i = 0;
    fprintf (f, "void\nfsm_%s_init ()\n{\n", fsm->name);
    while (sc != NULL)
      {
	fprintf (f, "\tfsm_%s_active_states[%u] = %u;\n",
		 fsm->name,
		 i,
		 sc->state.code);
	if (fsm->timeouts != NULL)
	  {
	    toc = fsm->timeouts;
	    while (toc != NULL)
	      {
		if (toc->timeout.trans->state->code == sc->state.code)
		  {
		    fprintf (f, "\tfsm_%s_timeout_counters[%u] = %u;\n",
			     fsm->name,
			     i,
			     toc->timeout.timeout);
		  }
		toc = toc->next;
	      }
	  }
	i++;
	sc = sc->next;
      }
    fprintf (f, "}\n\n");

    /* Gen handle function. */
    fprintf (f, "int\nfsm_%s_handle (fsm_%s_event_t e)\n{\n",
	     fsm->name,
	     fsm->name);
    fprintf (f, "\tuint16_t i;\n");
    fprintf (f, "\tint handled = 0;\n");
    fprintf (f, "\tfor (i = 0; i < fsm_%s_max_active_states; i++)\n\t{\n",
	     fsm->name);
    fprintf (f, "\t\tif (fsm_%s_read_trans (e, fsm_%s_active_states[i]))\n",
	     fsm->name,
	     fsm->name);
    fprintf (f, "\t\t{\n");
    fprintf (f, "\t\t\tfsm_%s_state_t old_state = fsm_%s_active_states[i];\n",
	     fsm->name,
	     fsm->name);
    fprintf (f, "\t\t\tfsm_%s_active_states[i] = fsm_%s_read_trans (e, "
             "fsm_%s_active_states[i])();\n",
             fsm->name,
             fsm->name,
             fsm->name);
    fprintf (f, "\t\t\tproto_send3b ('F', old_state, e, "
	     "fsm_%s_active_states[i]);\n",
	     fsm->name);
    fprintf (f, "\t\t\thandled = 1;\n");
    if (fsm->timeouts != NULL)
      {
	fprintf (f, "\t\t\tfsm_%s_timeout_counters[i] = "
		 "fsm_%s_timeout_values[fsm_%s_active_states[i]];\n",
		 fsm->name,
		 fsm->name,
		 fsm->name);
      }
    fprintf (f, "\t\t}\n");
    fprintf (f, "\t}\n");
    fprintf (f, "\treturn handled;\n");
    fprintf (f, "}\n\n");

    /* Gen can handle function. */
    fprintf (f, "uint16_t\nfsm_%s_can_handle (fsm_%s_event_t e)\n{\n",
	     fsm->name,
	     fsm->name);
    fprintf (f, "\tuint16_t i;\n");
    fprintf (f, "\tfor (i = 0; i < fsm_%s_max_active_states; i++)\n",
	     fsm->name);
    fprintf (f, "\t\tif (fsm_%s_read_trans (e, fsm_%s_active_states[i]))\n",
             fsm->name,
             fsm->name);
    fprintf (f, "\t\t\treturn 1;\n");
    fprintf (f, "\treturn 0;\n");
    fprintf (f, "}\n\n");

    if (fsm->timeouts != NULL)
      {
	/* Gen timeout counters array. */
	fprintf (f, "int32_t fsm_%s_timeout_counters[%u];\n",
		 fsm->name,
		 fsm->max_active_states);

	/* Gen timeout values array. */
	fprintf (f, "int32_t fsm_%s_timeout_values[FSM_STATE_%s_NB_] =\n{\n",
		 fsm->name,
		 fsm->name);
	int value;
	for (i = 0; i < fsm->state_nb; i++)
	  {
	    value = -1;
	    s = fsm_build_get_state_by_code (fsm, i);
	    assert (s);
	    toc = fsm->timeouts;
	    while (toc != NULL)
	      {
		if (s->code == toc->timeout.trans->state->code)
		  {
		    value = toc->timeout.timeout;
		    break;
		  }
		toc = toc->next;
	      }
	    fprintf (f, "\t%i", value);
	    if (i != fsm->state_nb - 1)
		fprintf (f, ",");
	    fprintf (f, "\n");
	  }
	fprintf (f, "};\n\n");

	/* Gen timeout corresponding events array. */
	fprintf (f, "fsm_%s_event_t fsm_%s_timeout_events[FSM_STATE_%s_NB_] =\n{\n",
		 fsm->name,
		 fsm->name,
		 fsm->name);
	for (i = 0; i < fsm->state_nb; i++)
	  {
	    value = -1;
	    s = fsm_build_get_state_by_code (fsm, i);
	    assert (s);
	    toc = fsm->timeouts;
	    while (toc != NULL)
	      {
		if (s->code == toc->timeout.trans->state->code)
		  {
		    value = toc->timeout.trans->event->code;
		    break;
		  }
		toc = toc->next;
	      }
	    if (value == -1)
		fprintf (f, "\tFSM_STATE_%s_NB_", fsm->name);
	    else
		fprintf (f, "\t%u", value);

	    if (i != fsm->state_nb - 1)
		fprintf (f, ",");
	    fprintf (f, "\n");
	  }
	fprintf (f, "};\n\n");

	/* Gen handle timeout function. */
	fprintf (f, "int\nfsm_%s_handle_timeout ()\n{\n",
		 fsm->name);
	fprintf (f, "\tuint16_t i;\n");
	fprintf (f, "\tint out = 0;\n");
	fprintf (f, "\tfor (i = 0; i < fsm_%s_max_active_states; i++)\n\t{\n",
		 fsm->name);
	fprintf (f, "\t\tif (fsm_%s_timeout_counters[i] > 0)\n\t\t{\n",
		 fsm->name);
	fprintf (f, "\t\t\tfsm_%s_timeout_counters[i]--;\n",
		 fsm->name);
	fprintf (f, "\t\t\tif (fsm_%s_timeout_counters[i] == 0)\n\t\t\t{\n",
		 fsm->name);
	fprintf (f, "\t\t\t\tfsm_%s_handle (fsm_%s_timeout_events[fsm_%s_active_states[i]]);\n",
		 fsm->name,
		 fsm->name,
		 fsm->name);
	fprintf (f, "\t\t\t\tout = 1;\n");
	fprintf (f, "\t\t\t}\n");
	fprintf (f, "\t\t}\n");
	fprintf (f, "\t}\n");
	fprintf (f, "\treturn out;\n");
	fprintf (f, "}\n\n");
      }

    /* Close file. */
    fclose (f);

    /* Free. */
    free (fn);
}

void
fsm_build_gen (char *arch, uint embedded_strings)
{
    fsm_build_chain_t *curs = fsm_build_all_fsm;
    while (curs != NULL)
      {
	if (strcmp (arch, "AVR") == 0)
	  {
	    fsm_build_gen_avr_h (curs->fsm, embedded_strings);
	    fsm_build_gen_avr_c (curs->fsm, embedded_strings);
	  }
	curs = curs->next;
      }
}

void
fsm_build_free (fsm_build_t *fsm)
{
    fsm_build_state_chain_t *sc;
    fsm_build_state_chain_t *sc_tmp;
    fsm_build_event_chain_t *ec;
    fsm_build_event_chain_t *ec_tmp;
    fsm_build_trans_chain_t *tc;
    fsm_build_trans_chain_t *tc_tmp;
    fsm_build_branch_chain_t *bc;
    fsm_build_branch_chain_t *bc_tmp;
    fsm_trans_func_chain_t *fc;
    fsm_trans_func_chain_t *fc_tmp;
    fsm_build_timeout_chain_t *toc;
    fsm_build_timeout_chain_t *toc_tmp;
    uint i;

    /* Free states. */
    sc = fsm->states;
    while (sc != NULL)
      {
	sc_tmp = sc;
	free (sc->state.var_name);
	sc = sc->next;
	free (sc_tmp);
      }

    /* Free events. */
    ec = fsm->events;
    while (sc != NULL)
      {
	ec_tmp = ec;
	free (ec->event.var_name);
	ec = ec->next;
	free (ec_tmp);
      }

    /* Free trans */
    tc = fsm->trans;
    while (tc != NULL)
      {
	tc_tmp = tc;

	/* Free each branches. */
	bc = tc->trans.output_branches;
	while (bc != NULL)
	  {
	    bc_tmp = bc;
	    free (bc->name);
	    bc = bc->next;
	    free (bc_tmp);
	  }

	tc = tc->next;
	free (tc_tmp);
      }

    /* Free start chain. */
    sc = fsm->starters;
    while (sc != NULL)
      {
	sc_tmp = sc;
	sc = sc->next;
	free (sc_tmp);
      }

    /* Free timeout chain. */
    toc = fsm->timeouts;
    while (toc != NULL)
      {
	toc_tmp = toc;
	toc = toc->next;
	free (toc_tmp);
      }

    /* Free run data (trans_table). */
    for (i = 0; i < fsm->event_nb; i++)
	free (fsm->run.trans_table[i]);
    free (fsm->run.trans_table);

    /* Free run data (active states). */
    free (fsm->run.active_states);

    /* Free last-seen event array*/
    free (fsm->run.events_before_active_state);

    /* Free run data (function pool). */
    fc = fsm->run.func_pool;
    while (fc != NULL)
      {
	fc_tmp = fc;
	fc = fc->next;
	free (fc_tmp);
      }

    /*Free run data (timeout counters). */
    free (fsm->run.timeout_counters);
}

