/*
   AngFSM - Almost Non Generated Finite State Machine
   Copyright 2011-2013 Jerome Jutteau

 This file is part of AngFSM.

 AngFSM is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 AngFSM is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with AngFSM. If not, see <http://www.gnu.org/licenses/>.
 */

#include "angfsm.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *angfsm_build_arch_name[ANGFSM_BUILD_ARCH_NB] = {
                                                            "host",
                                                            "avr",
                                                            "arm"
};

void
angfsm_build_print_help ()
{
   printf (
           "AngFSM Copyright (C) 2011-2013  Jérôme Jutteau\n"
           "This program comes with ABSOLUTELY NO WARRANTY;\n"
           "This is free software, and you are welcome to redistribute it\n"
           "under certain conditions; consult license for details.\n"
           "\n"
           "options:\n"
           "--ang-help : show this help.\n"
           "--ang-fsm [FSM_NAME]\n"
           "         Select a specific fsm by it's name, the next parameters will use\n"
           "         this parameter until another is set. You don't have to specifie\n"
           "         a fsm name with this option if there is only one fsm.\n"
           "--ang-dot [FILENAME]\n"
           "         Generate the dot representation of the current fsm in the\n"
           "         specified filename.\n"
           "--ang-gen [ARCH]\n"
           "         Generate a optimized code of all fsm for the specified\n"
           "         architecture. You will have to recompile your fsm with the\n"
           "         generated files but you won't have to edit your fsm code.\n"
           "         Available architecture are: "
           );
   int i;
   for (i = 0; i < ANGFSM_BUILD_ARCH_NB - 1; i++)
      printf ("%s, ", angfsm_build_arch_name[i]);
   printf ("%s.\n", angfsm_build_arch_name[ANGFSM_BUILD_ARCH_NB - 1]);
   printf (
           "         Once the code generated, you will have to recompile your code\n"
           "         and the generated .c files with the architecture's specific\n"
           "         compiler. If you want to recompile the fsm for the \"host\" target\n"
           "         you have to provide the -D __HOST_ARCH__ option to your compiler.\n"
           "--ang-embedded-strings\n"
           "         Add events and states strings in generated code for all fsm or\n"
           "         for a selected fsm. Put this option before --ang-gen.\n"
           "--ang-no-sanity-check\n"
           "         Disable sanity check during execution on the selected fsm or on\n"
           "         all fsm. Sanity checks are not embedded in generated code.\n"
           "--ang-no-print-transitions\n"
           "         Do not print transition to stdout each time one occurs.\n"
           "         Not available for generated code.\n"
           );
}

void
angfsm_build_arg_parse (char *string, char ***tab, int *nb)
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
         buff_cnt = 0;
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
angfsm_build_arg_free (char ***tab, int nb)
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
angfsm_build_print_trans (angfsm_build_t *fsm,
                          angfsm_build_trans_t *trans,
                          angfsm_build_branch_chain_t *branch)
{
   assert (fsm);
   assert (trans);
   assert (branch);
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
angfsm_build_sanity_check (angfsm_build_t *fsm)
{
   angfsm_build_event_chain_t *ec;
   angfsm_build_event_chain_t *ec_tmp;
   angfsm_build_state_chain_t *sc;
   angfsm_build_state_chain_t *sc_tmp;
   angfsm_build_trans_chain_t *tc;
   angfsm_build_trans_chain_t *tc_tmp;
   angfsm_build_branch_chain_t *bc;
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
   angfsm_build_state_chain_t *stack = NULL;
   angfsm_build_state_chain_t *tail = NULL;
   uint i;
   /* Initialise check array. */
   uint *check_tab = (uint *) malloc (fsm->state_nb * sizeof (uint));
   memset (check_tab, 0, fsm->state_nb * sizeof (uint));
   /* Initialize by adding first active states the stack. */
   sc = fsm->starters;
   while (sc != NULL)
   {
      sc_tmp = (angfsm_build_state_chain_t *)
         malloc (sizeof (angfsm_build_state_chain_t));
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
                  sc_tmp = (angfsm_build_state_chain_t *)
                     malloc (sizeof (angfsm_build_state_chain_t));
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
                  angfsm_build_get_state_by_code (fsm, i)->var_name);
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
}

void
angfsm_build_reset (angfsm_build_t *fsm)
{
   uint i;
   angfsm_build_state_chain_t *curs = fsm->starters;
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

   /* Reset timeouts. */
   for (i = 0; i < fsm->max_active_states; i++)
      fsm->run.timeout_counters[i] = -1;
   for (i = 0; i < fsm->max_active_states; i++)
   {
      angfsm_build_timeout_chain_t *toc = fsm->timeouts;
      while (toc != NULL)
      {
         if (fsm->run.active_states[i]->code
             == toc->timeout.trans->state->code)
            fsm->run.timeout_counters[i] = toc->timeout.timeout;
         toc = toc->next;
      }
   }
}

void
angfsm_build_gen_dot (angfsm_build_t *fsm, char* output)
{
   uint j;
   angfsm_build_trans_chain_t *tc;
   angfsm_build_branch_chain_t *bc;
   char *fn = NULL;
   if (!output || strlen (output) == 0)
   {
      uint fns = strlen (fsm->name) + strlen ("angfsm_.dot") + 1;
      char *fn = (char *) malloc (fns * sizeof (char));
      snprintf (fn, fns, "angfsm_%s.dot", fsm->name);
   }
   else
      fn = output;
   assert (fn);
   FILE *f = fopen (fn, "w");
   if (!f)
   {
      fprintf (stderr, "Warning: could not open %s\n", fn);
      return;
   }

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
   if (!output || strlen (output) == 0)
      free (fn);
}

/* All fsm. */
angfsm_build_chain_t *angfsm_build_all_fsm;

void
angfsm_build_init_all_fsm ()
{
   angfsm_build_all_fsm = NULL;
}

/* Just initialise structures. */
void
angfsm_build_init (angfsm_build_t *fsm, char *name)
{
   assert (fsm);
   fsm->events = NULL;
   fsm->states = NULL;
   fsm->trans = NULL;
   fsm->name = name;
   fsm->max_active_states = 0;
   fsm->max_events_per_states = 0;
   fsm->max_branches_per_trans = 1;
   fsm->state_nb = 0;
   fsm->event_nb = 0;
   fsm->starters = NULL;
   fsm->timeouts = NULL;
   fsm->u_branch_nb = 0;
   fsm->u_branch_name = NULL;

   fsm->run.trans_table = NULL;
   fsm->run.active_states = NULL;
   fsm->run.events_before_active_state = NULL;
   fsm->run.func_pool = NULL;
   fsm->run.timeout_counters = NULL;
   fsm->run.trans_callback = NULL;

   /* Set default user's options. */
   fsm->options.embedded_strings = 0;
   fsm->options.sanity_check = 1;
   fsm->options.gen_dot = NULL;
   fsm->options.gen_code = NULL;
   fsm->options.print_trans = 1;

   /* Store fsm. */
   angfsm_build_chain_t *niou = (angfsm_build_chain_t *)
      malloc (sizeof (angfsm_build_chain_t));
   niou->fsm = fsm;
   niou->next = angfsm_build_all_fsm;
   angfsm_build_all_fsm = niou;
}

/* Compute last details about fsm. */
void
angfsm_build_init_finalize (angfsm_build_t *fsm)
{
   uint i, j, k;
   uint *cpt;
   char **t;
   angfsm_build_trans_chain_t *tc;
   angfsm_build_branch_chain_t *bc;
   assert (fsm);

   /* Compute maximal number of events per states. */
   cpt = (uint *) calloc (fsm->state_nb, sizeof (uint));
   tc = fsm->trans;
   while (tc != NULL)
   {
       cpt[tc->trans.state->code]++;
       tc = tc->next;
   }
   for (i = 0; i < fsm->state_nb; i++)
       if (cpt[i] > fsm->max_events_per_states)
           fsm->max_events_per_states = cpt[i];
   free (cpt);

   /* Populate every unique branch names. */
   /* 1. count the number of branch with a name. */
   j = 0;
   tc = fsm->trans;
   while (tc != NULL)
   {
      bc = tc->trans.output_branches;
      while (bc != NULL)
      {
         if (bc->name != NULL)
             j++;
         bc = bc->next;
      }
      tc = tc->next;
   }
   /* 2. Create temporary array and fill it. */
   if (j > 0)
   {
       t = (char **) calloc (j, sizeof (char *));
       k = 0;
       tc = fsm->trans;
       /* In all transitions. */
       while (tc != NULL)
       {
           bc = tc->trans.output_branches;
           /* In all branches. */
           while (bc != NULL)
           {
               if (bc->name != NULL)
               {
                   /* Check if this name already exists. */
                   int exists = 0;
                   for (i = 0; i < j; i++)
                       if (t[i] && strcmp (bc->name, t[i]) == 0)
                           exists = 1;
                   /* Add to array. */
                   if (!exists)
                   {
                       t[k] = bc->name;
                       k++;
                   }
               }
               bc = bc->next;
           }
           tc = tc->next;
       }
       /* 3. Rescale the array and store it. */
       if (k < j)
           t = (char **) realloc (t, k * sizeof (char *));
       fsm->u_branch_nb = k;
       fsm->u_branch_name = t;
   }
}

/* Prepare the fsm to run (at very end). */
void
angfsm_build_run_init (angfsm_build_t *fsm)
{
   uint i, j;

   /* Create empty transition table. */
   fsm->run.trans_table = (angfsm_build_run_strans_func_t **)
      malloc (fsm->event_nb * sizeof (angfsm_build_run_strans_func_t *));
   for (i = 0; i < fsm->event_nb; i++)
   {
      fsm->run.trans_table[i] = (angfsm_build_run_strans_func_t *)
         malloc (fsm->state_nb * sizeof (angfsm_build_run_strans_func_t));
      for (j = 0; j < fsm->state_nb; j++)
         fsm->run.trans_table[i][j] = NULL;
   }

   /* Fill with transitions. */
   angfsm_trans_func_chain_t *fc = fsm->run.func_pool;
   while (fc != NULL)
   {
      fsm->run.trans_table[fc->trans.event->code][fc->trans.state->code] =
         fc->func;
      fc = fc->next;
   }

   /* Free function pool. */
   angfsm_trans_func_chain_t *old_fc;
   fc = fsm->run.func_pool;
   while (fc != NULL)
   {
      old_fc = fc;
      fc = fc->next;
      free (old_fc);
   }
   fsm->run.func_pool = NULL;

   /* Initialize active states. */
   fsm->run.active_states = (angfsm_build_state_t **) malloc
      (fsm->max_active_states * sizeof (angfsm_build_state_t *));
   for (i = 0; i < fsm->max_active_states; i++)
      fsm->run.active_states[i] = NULL;
   angfsm_build_state_chain_t *s = fsm->starters;
   i = 0;
   while (s != NULL)
   {
      fsm->run.active_states[i++] = &s->state;
      s = s->next;
   }

   /* Initialize last events before active states. */
   fsm->run.events_before_active_state = (angfsm_build_event_t **) malloc
      (fsm->max_active_states * sizeof (angfsm_build_event_t *));
   for (i = 0; i < fsm->max_active_states; i++)
      fsm->run.events_before_active_state[i] = NULL;

   /* Initialize timeout counters. */
   fsm->run.timeout_counters = (int *)
      malloc (fsm->max_active_states * sizeof (int));
   for (i = 0; i < fsm->max_active_states; i++)
      fsm->run.timeout_counters[i] = -1;
   for (i = 0; i < fsm->max_active_states; i++)
   {
      angfsm_build_timeout_chain_t *toc = fsm->timeouts;
      while (toc != NULL)
      {
         if (fsm->run.active_states[i]->code
             == toc->timeout.trans->state->code)
            fsm->run.timeout_counters[i] = toc->timeout.timeout;
         toc = toc->next;
      }
   }
}

void
angfsm_build_states (angfsm_build_t *fsm, char *states)
{
   angfsm_build_state_chain_t *head = fsm->states;
   char **args;
   int i, nb;

   angfsm_build_arg_parse (states, &args, &nb);
   for (i = 0; i < nb; i++)
   {
      angfsm_build_state_chain_t *s = (angfsm_build_state_chain_t *)
         malloc (sizeof (angfsm_build_state_chain_t));
      s->state.var_name = strdup (args[i]);
      s->state.code = fsm->state_nb++;
      s->next = head;
      head = s;
   }
   angfsm_build_arg_free (&args, nb);
   fsm->states = head;
}

void
angfsm_build_events (angfsm_build_t *fsm, char *events)
{
   angfsm_build_event_chain_t *head = fsm->events;
   char **args;
   int i, nb;

   if (strcmp (events, "_TIMEOUT_") == 0
       && angfsm_build_get_event (fsm, events) != NULL)
      return;
   angfsm_build_arg_parse (events, &args, &nb);
   for (i = 0; i < nb; i++)
   {
      angfsm_build_event_chain_t *e = (angfsm_build_event_chain_t *)
         malloc (sizeof (angfsm_build_event_chain_t));
      e->event.var_name = strdup (args[i]);
      e->event.code = fsm->event_nb++;
      e->next = head;
      head = e;
   }
   angfsm_build_arg_free (&args, nb);
   fsm->events = head;
}

angfsm_build_event_t *
angfsm_build_get_event (angfsm_build_t *fsm, char *event)
{
   angfsm_build_event_chain_t *curs = fsm->events;
   while (curs != NULL)
   {
      if (strcmp (curs->event.var_name, event) == 0)
         return &(curs->event);
      curs = curs->next;
   }
   return NULL;
}

angfsm_build_state_t *
angfsm_build_get_state (angfsm_build_t *fsm, char *state)
{
   angfsm_build_state_chain_t *curs = fsm->states;
   while (curs != NULL)
   {
      if (strcmp (curs->state.var_name, state) == 0)
         return &(curs->state);
      curs = curs->next;
   }
   return NULL;
}

angfsm_build_event_t*
angfsm_build_get_event_by_code (angfsm_build_t *fsm, uint event)
{
   angfsm_build_event_chain_t *curs = fsm->events;
   while (curs != NULL)
   {
      if (curs->event.code == event)
         return &(curs->event);
      curs = curs->next;
   }
   return NULL;
}

angfsm_build_state_t*
angfsm_build_get_state_by_code (angfsm_build_t *fsm, uint state)
{
   angfsm_build_state_chain_t *curs = fsm->states;
   while (curs != NULL)
   {
      if (curs->state.code == state)
         return &(curs->state);
      curs = curs->next;
   }
   return NULL;
}

angfsm_build_trans_t*
angfsm_build_get_trans (angfsm_build_t *fsm, uint state, uint event)
{
   assert (fsm);
   assert (state < fsm->state_nb);
   assert (event < fsm->event_nb);

   angfsm_build_trans_chain_t *tc = fsm->trans;
   while (tc != NULL)
   {
      if (tc->trans.event->code == event && tc->trans.state->code == state)
          return &(tc->trans);
      tc = tc->next;
   }
   return NULL;
}

uint16_t
angfsm_build_get_event_code (angfsm_build_t *fsm, char *event)
{
   assert (fsm);
   angfsm_build_event_t *e = angfsm_build_get_event (fsm, event);
   assert (e);
   return e->code;
}

angfsm_build_branch_chain_t*
angfsm_build_get_event_branch (angfsm_build_t *fsm,
                               angfsm_build_trans_t *trans,
                               uint branch)
{
   assert (fsm);
   assert (trans);
   angfsm_build_branch_chain_t *b = trans->output_branches;
   while (b != NULL)
   {
      if (angfsm_build_get_branch (fsm, b->name) == branch)
         break;
      b = b->next;
   }
   return b;
}

uint
angfsm_build_get_branch (angfsm_build_t *fsm, char *branch)
{
   assert (fsm);
   uint i;
   if (branch && strlen (branch) > 0)
       for (i = 0; i < fsm->u_branch_nb; i++)
           if (strcmp (branch, fsm->u_branch_name[i]) == 0)
               return i;
   return fsm->u_branch_nb;
}

void
angfsm_build_trans (angfsm_build_t *fsm,
                    char *state,
                    char *event,
                    char *output_branches,
                    angfsm_build_run_strans_func_t trans_func)
{
   angfsm_build_trans_t t;
   angfsm_build_branch_chain_t *b;
   char **args;
   int i, nb;

   t.output_branches = NULL;
   /* Check state and event exists. */
   t.state = angfsm_build_get_state (fsm, state);
   t.event = angfsm_build_get_event (fsm, event);
   assert (t.state);
   assert (t.event);

   angfsm_build_arg_parse (output_branches, &args, &nb);

   /* Are we the winner of the maximal branches per transitions ? */
   if ((uint) ((nb / 2)) > fsm->max_branches_per_trans)
       fsm->max_branches_per_trans = nb / 2;

   /* Only one output state. */
   if (nb == 1)
   {
      t.output_branches = (angfsm_build_branch_chain_t *)
         malloc (sizeof (angfsm_build_branch_chain_t));
      t.output_branches->name = NULL;
      t.output_branches->state = angfsm_build_get_state (fsm, args[0]);
      assert (t.output_branches->state);
      t.output_branches->next = NULL;
   }
   else
      for (i = 0; i < nb; i = i + 2)
      {
         b = (angfsm_build_branch_chain_t *)
            malloc (sizeof (angfsm_build_branch_chain_t));
         b->name = strdup (args[i]);
         b->state = angfsm_build_get_state (fsm, args[i + 1]);
         if (!b->state)
             fprintf (stderr, "Error: the state \"%s\" has not been declared in fsm %s.\n",
                       args[i + 1], fsm->name);
         assert (b->state);
         b->next = t.output_branches;
         t.output_branches = b;
      }
   angfsm_build_arg_free (&args, nb);

   /* Add trans to fsm. */
   angfsm_build_trans_chain_t *tc = (angfsm_build_trans_chain_t *)
      malloc (sizeof (angfsm_build_trans_chain_t));
   tc->trans = t;
   tc->next = fsm->trans;
   fsm->trans = tc;

   /* Add trans function to run context. */
   angfsm_trans_func_chain_t *fc = (angfsm_trans_func_chain_t *)
      malloc (sizeof (angfsm_trans_func_chain_t));
   fc->func = trans_func;
   fc->trans = tc->trans;
   fc->next = fsm->run.func_pool;
   fsm->run.func_pool = fc;
}

void
angfsm_build_timeout (angfsm_build_t *fsm, char *state, char *event, uint timeout)
{
   angfsm_build_trans_chain_t *tc = NULL;
   angfsm_build_trans_t *t = NULL;
   angfsm_build_state_t *s = NULL;
   angfsm_build_event_t *e = NULL;
   angfsm_build_timeout_chain_t *to = NULL;

   assert (fsm);
   assert (state);
   assert (event);

   /* Find the corresponding transition. */
   tc = fsm->trans;
   s = angfsm_build_get_state (fsm, state);
   e = angfsm_build_get_event (fsm, event);
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
   to = (angfsm_build_timeout_chain_t *)
      malloc (sizeof (angfsm_build_timeout_chain_t));
   to->timeout.timeout = timeout;
   to->timeout.trans = t;

   /* Add timeout to chain. */
   to->next = fsm->timeouts;
   fsm->timeouts = to;
}

void
angfsm_build_start_with (angfsm_build_t *fsm, char *starters)
{
   char **args;
   int nb, i;
   angfsm_build_arg_parse (starters, &args, &nb);
   fsm->max_active_states += nb;
   for (i = 0; i < nb; i++)
   {
      angfsm_build_state_chain_t *sc = (angfsm_build_state_chain_t *)
         malloc (sizeof (angfsm_build_state_chain_t));
      angfsm_build_state_t *s = angfsm_build_get_state (fsm, args[i]);
      assert (s);
      sc->state = *s;
      sc->next = fsm->starters;
      fsm->starters = sc;
   }
   angfsm_build_arg_free (&args, nb);
}

int
angfsm_build_handle (angfsm_build_t *fsm, angfsm_build_event_t *e)
{
   angfsm_build_state_t *s = NULL;
   angfsm_build_timeout_chain_t *toc = NULL;
   angfsm_build_state_t *out;
   angfsm_build_branch_chain_t* bc;
   assert (e);
   uint i;
   uint b = 0;
   int mono;
   int handled = 0;
   for (i = 0; i < fsm->max_active_states; i++)
   {
      s = fsm->run.active_states[i];
      if (s && fsm->run.trans_table[e->code][s->code])
      {
         fsm->run.events_before_active_state[i] = e;

         /* Get transition. */
         angfsm_build_trans_t *t = angfsm_build_get_trans (fsm, s->code, e->code);
         assert (t);

         /* Is it a mono or multi branch function ? */
         assert (t->output_branches);
         if (t->output_branches->next == NULL)
            mono = 1;
         else
            mono = 0;

         /* Get transition function. */
         angfsm_build_run_strans_func_t f = fsm->run.trans_table[e->code][s->code];
         angfsm_build_run_strans_func_branches_t fb = (angfsm_build_run_strans_func_branches_t) f;

         /* Run transition. */
         if (mono)
             f ();
         else
             b = fb();

         /* Get next branch. */
         bc = NULL;
         if (mono)
            bc = t->output_branches;
         else
            bc = angfsm_build_get_event_branch (fsm, t, b);

         if (bc == NULL)
             fprintf (stderr, "FSM: %s STATE: %s EVENT: %s - Cannot handle given branch \"%s\" (%s)\n", fsm->name, s->var_name, e->var_name, (b < fsm->u_branch_nb ? fsm->u_branch_name[b] : ""), (b < fsm->u_branch_nb ? "wrong branch ?" : "bad branch name ?"));
         assert (bc);
         out = bc->state;
         assert (out);

         /* Transition print. */
         if (fsm->options.print_trans)
            angfsm_build_print_trans (fsm, t, bc);

         /* Transition callback. */
         if (fsm->run.trans_callback)
            fsm->run.trans_callback (s->code, e->code, out->code, (mono ? -1 : (int) b));

         /* Update active states. */
         fsm->run.active_states[i] = out;

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
angfsm_build_handle_string (angfsm_build_t *fsm, char *event)
{
   angfsm_build_event_t *e = angfsm_build_get_event (fsm, event);
   return angfsm_build_handle (fsm, e);
}

int
angfsm_build_handle_integer (angfsm_build_t *fsm, uint16_t event)
{
   angfsm_build_event_t *e = angfsm_build_get_event_by_code (fsm, event);
   return angfsm_build_handle (fsm, e);
}

int
angfsm_build_can_handle (angfsm_build_t *fsm, angfsm_build_event_t *e)
{
   angfsm_build_state_t *s = NULL;
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
angfsm_build_can_handle_string (angfsm_build_t *fsm, char *event)
{
   angfsm_build_event_t *e = angfsm_build_get_event (fsm, event);
   return angfsm_build_can_handle (fsm, e);
}

int
angfsm_build_can_handle_integer (angfsm_build_t *fsm, uint16_t event)
{
   angfsm_build_event_t *e = angfsm_build_get_event_by_code (fsm, event);
   return angfsm_build_can_handle (fsm, e);
}

int
angfsm_build_handle_timeout (angfsm_build_t *fsm)
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
         angfsm_build_handle_string (fsm, event);
         out = 1;
      }
   }
   return out;
}

int
angfsm_build_options (int argc, char **argv)
{
   assert (argv);
   int has_option = 0;
   int i;
   if (argc <= 1)
      return has_option;

   /* Select 1st FSM if it is the only one. */
   angfsm_build_t *fsm = NULL;
   angfsm_build_chain_t *fc = angfsm_build_all_fsm;
   if (fc && fc->next == NULL)
      fsm = fc->fsm;

   /* Option parsing. Ok it's dirty. */
   for (i = 1; i < argc; i++)
   {
      /* Print help. */
      if (strcmp ("--ang-help", argv[i]) == 0)
      {
         angfsm_build_print_help ();
         has_option = 1;
      }

      /* Select a specific FSM. */
      if (strcmp ("--ang-fsm", argv[i]) == 0 && i + 1 < argc)
      {
         fc = angfsm_build_all_fsm;
         while (fc != NULL)
         {
            if (strcmp (fc->fsm->name, argv[i + 1]) == 0)
            {
               fsm = fc->fsm;
               i++;
               break;
            }
            fc = fc->next;
         }
         has_option = 1;
      }

      /* Generate dot file. */
      if (strcmp ("--ang-dot", argv[i]) == 0 && i + 1 < argc && fsm)
      {
         fsm->options.gen_dot = argv[++i];
         has_option = 1;
      }

      /* Generate optimized code for specific architectures. */
      if (strcmp ("--ang-gen", argv[i]) == 0 && i + 1 < argc)
      {
         if (fsm)
            fsm->options.gen_code = argv[i + 1];
         else
         {
            fc = angfsm_build_all_fsm;
            while (fc != NULL)
            {
               fc->fsm->options.gen_code = argv[i + 1];
               fc = fc->next;
            }
         }
         i++;
         has_option = 1;
      }

      /* Embed strings in generated code. */
      if (strcmp ("--ang-embedded-strings", argv[i]) == 0)
      {
         if (fsm)
            fsm->options.embedded_strings = 1;
         else
         {
            fc = angfsm_build_all_fsm;
            while (fc != NULL)
            {
               fc->fsm->options.embedded_strings = 1;
               fc = fc->next;
            }
         }
         has_option = 1;
      }

      /* No sanity check for a specific fsm or for all fsm. */
      if (strcmp ("--ang-no-sanity-check", argv[i]) == 0)
      {
         if (fsm)
            fsm->options.sanity_check = 0;
         else
         {
            fc = angfsm_build_all_fsm;
            while (fc != NULL)
            {
               fc->fsm->options.sanity_check = 0;
               fc = fc->next;
            }
         }
      }

      /* Print transitions each time one occurs. */
      if (strcmp ("--ang-no-print-transitions", argv[i]) == 0)
      {
         if (fsm)
            fsm->options.print_trans = 0;
         else
         {
            fc = angfsm_build_all_fsm;
            while (fc != NULL)
            {
               fc->fsm->options.print_trans = 0;
               fc = fc->next;
            }
         }
      }
   }

   /* Option execution. */
   if (has_option)
   {
      fc = angfsm_build_all_fsm;
      while (fc != NULL)
      {
         if (fc->fsm->options.gen_dot)
            angfsm_build_gen_dot (fc->fsm, fc->fsm->options.gen_dot);
         if (fc->fsm->options.gen_code)
            angfsm_build_gen (fc->fsm->options.gen_code);
         if (fc->fsm->options.sanity_check)
            angfsm_build_sanity_check (fc->fsm);
         fc = fc->next;
      }
   }
   return has_option;
}

void
angfsm_build_gen_no_opti_h (angfsm_build_t *fsm, angfsm_build_arch_t arch)
{
   assert (arch < ANGFSM_BUILD_ARCH_NB);
   angfsm_build_state_chain_t *sc;
   angfsm_build_event_chain_t *ec;
   angfsm_build_trans_chain_t *tc;
   angfsm_build_chain_t *all_fsm;
   uint i;
   uint embedded_strings = fsm->options.embedded_strings;

   /* Open file. */
   char *fn = (char*) malloc ((strlen (fsm->name)
                               + strlen ("angfsm_gen__.h")
			       + strlen (angfsm_build_arch_name[arch])
			       + 1) * sizeof (char));
   sprintf (fn, "angfsm_gen_%s_%s.h", angfsm_build_arch_name[arch], fsm->name);
   FILE *f = fopen (fn, "w");

   /* Introduction. */
   fprintf (f, "/* This file has been generated, do not edit. */\n\n");
   fprintf (f, "#ifndef _angfsm_%s_\n", fsm->name);
   fprintf (f, "#define _angfsm_%s_\n\n", fsm->name);
   fprintf (f, "#include <inttypes.h>\n\n");
   fprintf (f, "#include <string.h>\n\n");

   /* Include all other fsm headers. */
   all_fsm = angfsm_build_all_fsm;
   while (all_fsm != NULL)
   {
      fprintf (f, "#include \"angfsm_gen_%s_%s.h\"\n",
	       angfsm_build_arch_name[arch],
	       all_fsm->fsm->name);
      all_fsm = all_fsm->next;
   }

   /* Gen max active states define */
   fprintf (f, "#define angfsm_%s_max_active_states %u\n",
            fsm->name,
            fsm->max_active_states);

   /* Gen max number of events per states. */
   fprintf (f, "#define angfsm_%s_max_events_per_states %u\n",
            fsm->name,
            fsm->max_events_per_states);

   /* Gen max number of branches per transitions. */
   fprintf (f, "#define angfsm_%s_max_branches_per_trans %u\n",
            fsm->name,
            fsm->max_branches_per_trans);

   /* Gen state enum. */
   fprintf (f, "typedef enum\n{\n");
   sc = fsm->states;
   while (sc != NULL)
   {
      fprintf (f, "\tangfsm_STATE_%s_%s = %u,\n", fsm->name, sc->state.var_name, sc->state.code);
      sc = sc->next;
   }
   fprintf (f, "\tangfsm_STATE_%s_NB = %u\n", fsm->name, fsm->state_nb);
   fprintf (f, "} angfsm_%s_state_t;\n\n", fsm->name);

   /* Gen transition callback reference. */
   fprintf (f, "extern void (*angfsm_%s_trans_callback) (int state, int event, int output_branch, int branch);\n", fsm->name);

   /* Gen event enum. */
   fprintf (f, "typedef enum\n{\n");
   ec = fsm->events;
   while (ec != NULL)
   {
      fprintf (f, "\tangfsm_EVENT_%s_%s = %u,\n", fsm->name, ec->event.var_name, ec->event.code);
      ec = ec->next;
   }
   fprintf (f, "\tangfsm_EVENT_%s_NB = %u\n", fsm->name, fsm->event_nb);
   fprintf (f, "} angfsm_%s_event_t;\n\n", fsm->name);

   /* Gen branches unique name enum. */
   fprintf (f, "typedef enum\n{\n");
   if (fsm->u_branch_nb > 0)
   {
      for (i = 0; i < fsm->u_branch_nb; i++)
          fprintf (f, "\tangfsm_BRANCH_%s_%s = %u,\n",
                   fsm->name,
                   fsm->u_branch_name[i],
                   i);
   }
   fprintf (f, "\tangfsm_BRANCH_%s_NB = %u\n", fsm->name, fsm->u_branch_nb);
   fprintf (f, "} angfsm_%s_branch_t;\n\n", fsm->name);

   /* Gen strings. */
   if (embedded_strings)
   {
      fprintf (f, "extern char *angfsm_%s_state_str[];\n", fsm->name);
      fprintf (f, "extern char *angfsm_%s_event_str[];\n", fsm->name);
      fprintf (f, "extern char *angfsm_%s_branch_str[];\n", fsm->name);

      /* Convert an event in string. */
      fprintf (f, "char *\nangfsm_%s_get_event_str (angfsm_%s_event_t e);\n", fsm->name, fsm->name);
      /* Convert a state in string. */
      fprintf (f, "char *\nangfsm_%s_get_state_str (angfsm_%s_state_t s);\n", fsm->name, fsm->name);
      /* Convert a branch in string. */
      fprintf (f, "char *\nangfsm_%s_get_branch_str (angfsm_%s_branch_t s);\n", fsm->name, fsm->name);

      fprintf (f, "\n");
   }
   else
   {
      /* Disable string macros. */
      fprintf (f, "#undef ANGFSM_STATE_STR\n#define ANGFSM_STATE_STR(s) ((char *)0)\n");
      fprintf (f, "#undef ANGFSM_EVENT_STR\n#define ANGFSM_EVENT_STR(e) ((char *)0)\n");
      fprintf (f, "#undef ANGFSM_BRANCH_STR\n#define ANGFSM_BRANCH_STR(b) ((char *)0)\n");
   }

   /* Gen function headers. */
   tc = fsm->trans;
   while (tc != NULL)
   {
      assert (tc->trans.output_branches);
      /* Mono branch */
      if (tc->trans.output_branches->next==NULL)
         fprintf (f, "void angfsm_%s_trans_func_%s_%s ();\n",
                  fsm->name,
                  tc->trans.state->var_name,
                  tc->trans.event->var_name);
      else
         fprintf (f, "angfsm_%s_branch_t angfsm_%s_trans_func_%s_%s ();\n",
                  fsm->name,
                  fsm->name,
                  tc->trans.state->var_name,
                  tc->trans.event->var_name);
      tc = tc->next;
   }
   fprintf (f, "\n");

   /* Declare function type. */
   fprintf (f, "typedef void (*angfsm_%s_func_t)();\n", fsm->name);
   fprintf (f, "typedef angfsm_%s_branch_t (*angfsm_%s_func_branches_t)();\n\n",
            fsm->name, fsm->name);

   /* Gen branch structure. */
   fprintf (f, "typedef struct\n{\n");
   fprintf (f, "\tangfsm_%s_branch_t branch;\n", fsm->name);
   fprintf (f, "\tangfsm_%s_state_t state;\n", fsm->name);
   fprintf (f, "} angfsm_%s_next_t;\n\n", fsm->name);

   /* Gen transition structure. */
   fprintf (f, "typedef struct\n{\n");
   fprintf (f, "\tangfsm_%s_event_t event;\n", fsm->name);
   fprintf (f, "\tangfsm_%s_func_t func;\n", fsm->name);
   fprintf (f, "\tangfsm_%s_next_t branches[angfsm_%s_max_branches_per_trans];\n",
            fsm->name,
            fsm->name);
   fprintf (f, "} angfsm_%s_trans_t;\n\n", fsm->name);

   /* Gen transition table. */
   fprintf (f, "extern const angfsm_%s_trans_t angfsm_%s_trans_table[angfsm_STATE_%s_NB][angfsm_%s_max_events_per_states];\n\n",
            fsm->name,
            fsm->name,
            fsm->name,
            fsm->name);

   /* Gen read function for trans table. */
   fprintf (f, "inline angfsm_%s_trans_t* angfsm_%s_read_trans (angfsm_%s_event_t event, angfsm_%s_state_t state);\n\n",
            fsm->name,
            fsm->name,
            fsm->name,
            fsm->name);

   /* Gen active states array. */
   fprintf (f, "extern angfsm_%s_state_t angfsm_%s_active_states[angfsm_%s_max_active_states];\n\n",
            fsm->name,
            fsm->name,
            fsm->name);

   /* Gen initialization function. */
   sc = fsm->starters;
   i = 0;
   fprintf (f, "void\nangfsm_%s_init () __attribute__ ((constructor));\n\n",
            fsm->name);

   /* Gen handle function. */
   fprintf (f, "int\nangfsm_%s_handle (angfsm_%s_event_t e);\n",
            fsm->name,
            fsm->name);

   /* Gen can handle function. */
   fprintf (f, "uint16_t\nangfsm_%s_can_handle (angfsm_%s_event_t e);\n\n",
            fsm->name,
            fsm->name);

   if (fsm->timeouts != NULL)
   {
      /* Gen handle timeout function. */
      fprintf (f, "int\nangfsm_%s_handle_timeout ();\n",
               fsm->name);

      /* Gen timeout values. */
      fprintf (f, "extern int32_t angfsm_%s_timeout_values[angfsm_STATE_%s_NB];\n",
               fsm->name,
               fsm->name);

      /* Gen timeout corresponding events. */
      fprintf (f, "extern angfsm_%s_event_t angfsm_%s_timeout_events[angfsm_STATE_%s_NB];\n",
               fsm->name,
               fsm->name,
               fsm->name);

      /* Gen timeout counters array. */
      fprintf (f, "extern int32_t angfsm_%s_timeout_counters[angfsm_%s_max_active_states];\n\n",
               fsm->name,
               fsm->name);
   }

   /* Conclusion. */
   fprintf (f, "#endif /* #ifndef _angfsm_%s_ */", fsm->name);

   /* Close file. */
   fclose (f);
   free (fn);
}

void
angfsm_build_gen_no_opti_c (angfsm_build_t *fsm, angfsm_build_arch_t arch)
{
   assert (fsm);
   assert (arch < ANGFSM_BUILD_ARCH_NB);
   angfsm_build_state_chain_t *sc;
   angfsm_build_event_chain_t *ec;
   angfsm_build_trans_chain_t *tc;
   angfsm_build_timeout_chain_t *toc;
   angfsm_build_branch_chain_t *bc;
   angfsm_build_state_t *s;
   uint i, j, k, count;
   uint embedded_strings = fsm->options.embedded_strings;

   /* Open file. */
   char *fn = (char*) malloc ((strlen (fsm->name)
                               + strlen ("angfsm_gen__.c")
			       + strlen (angfsm_build_arch_name[arch])
			       + 1) * sizeof (char));
   sprintf (fn, "angfsm_gen_%s_%s.c", angfsm_build_arch_name[arch], fsm->name);
   FILE *f = fopen (fn, "w");

   /* Introduction. */
   fprintf (f, "/* This file has been generated, do not edit. */\n\n");
   fprintf (f, "#include \"angfsm_gen_%s_%s.h\"\n\n",
	    angfsm_build_arch_name[arch],
	    fsm->name);

   /* Gen strings if configured. */
   if (embedded_strings)
   {
      /* Gen state strings. */
      fprintf (f, "char *angfsm_%s_state_str[] =\n{\n", fsm->name);
      for (i = 0; i < fsm->state_nb; i++)
      {
          sc = fsm->states;
          while (sc != NULL)
          {
              if (sc->state.code == i)
              {
                  fprintf (f, "\t\"%s\"", sc->state.var_name);
                  if (i != fsm->state_nb)
                      fprintf (f, ", ");
                  fprintf (f, "\n");
              }
              sc = sc->next;
          }
      }
      fprintf (f, "};\n\n");

      /* Gen event strings. */
      fprintf (f, "char *angfsm_%s_event_str[] =\n{\n", fsm->name);
      for (i = 0; i < fsm->event_nb; i++)
      {
          ec = fsm->events;
          while (ec != NULL)
          {
              if (ec->event.code == i)
              {
                  fprintf (f, "\t\"%s\"", ec->event.var_name);
                  if (i != fsm->event_nb)
                      fprintf (f, ", ");
                  fprintf (f, "\n");
              }
              ec = ec->next;
          }
      }
      fprintf (f, "};\n\n");

      /* Gen branch strings. */
      fprintf (f, "char *angfsm_%s_branch_str[] =\n{\n", fsm->name);
      for (i = 0; i < fsm->u_branch_nb; i++)
      {
         fprintf (f, "\t\"%s\"", fsm->u_branch_name[i]);
         if (i != fsm->u_branch_nb - 1)
             fprintf (f, ", ");
         fprintf (f, "\n");
      }
      fprintf (f, "};\n\n");

      /* Generate string read functions. */
      /* Convert an event enum in string. */
      fprintf (f, "inline char *\nangfsm_%s_get_event_str (angfsm_%s_event_t e)\n{\n", fsm->name, fsm->name);
      fprintf (f, "\treturn angfsm_%s_event_str[e];\n", fsm->name);
      fprintf (f, "}\n\n");
      /* Convert a state enum in string. */
      fprintf (f, "inline char *\nangfsm_%s_get_state_str (angfsm_%s_state_t s)\n{\n", fsm->name, fsm->name);
      fprintf (f, "\treturn angfsm_%s_state_str[s];\n", fsm->name);
      fprintf (f, "}\n\n");
      /* Convert a branch enum in string. */
      fprintf (f, "inline char *\nangfsm_%s_get_branch_str (angfsm_%s_branch_t b)\n{\n", fsm->name, fsm->name);
      fprintf (f, "\treturn angfsm_%s_branch_str[b];\n", fsm->name);
      fprintf (f, "}\n\n");
   }

   /* Gen transition table. */
   fprintf (f, "const angfsm_%s_trans_t angfsm_%s_trans_table[angfsm_STATE_%s_NB][angfsm_%s_max_events_per_states] = \n{\n",
            fsm->name,
            fsm->name,
            fsm->name,
            fsm->name);
   for (i = 0; i < fsm->state_nb; i++)
   {
      count = 0;
      fprintf (f, "\t{\n");
      tc = fsm->trans;
      while (tc != NULL)
      {
          if (tc->trans.state->code == i)
          {
             fprintf (f, "\t\t{\n");
             fprintf (f, "\t\t\tangfsm_EVENT_%s_%s,\n",
                      fsm->name,
                      tc->trans.event->var_name);
             fprintf (f, "\t\t\t(angfsm_%s_func_t) &angfsm_%s_trans_func_%s_%s,\n",
                      fsm->name,
                      fsm->name,
                      tc->trans.state->var_name,
                      tc->trans.event->var_name);
             /* Gen branche array */
             fprintf (f, "\t\t\t{\n");
             j = 0;
             bc = tc->trans.output_branches;
             while (bc != NULL)
             {
                fprintf (f, "\t\t\t\t{angfsm_BRANCH_%s_%s, angfsm_STATE_%s_%s}",
                         fsm->name,
                         (bc->name ? bc->name : "NB"),
                         fsm->name,
                         bc->state->var_name);
                if (j != fsm->max_branches_per_trans - 1)
                    fprintf (f, ", ");
                fprintf (f, "\n");
                j++;
                bc = bc->next;
             }
             /* Fill blank branches. */
             for (k = j; k < fsm->max_branches_per_trans; k++)
             {
                 fprintf (f, "\t\t\t\t{angfsm_BRANCH_%s_NB, angfsm_STATE_%s_NB}",
                          fsm->name,
                          fsm->name);
                 if (k != fsm->max_branches_per_trans - 1)
                    fprintf (f, ",");
                 fprintf (f, "\n");
             }

             fprintf (f, "\t\t\t}\n");
             if (tc->next != NULL || count != fsm->max_events_per_states)
                 fprintf (f, "\t\t},\n");
             else
                 fprintf (f, "\t\t}\n");
             count++;
          }
          tc = tc->next;
      }
      /* Fill blank transitions. */
      for (k = count; k < fsm->max_events_per_states; k++)
      {
          fprintf (f, "\t\t{\n");
          fprintf (f, "\t\t\tangfsm_EVENT_%s_NB,\n",fsm->name);
          fprintf (f, "\t\t\t(angfsm_%s_func_t) 0,\n", fsm->name);
          fprintf (f, "\t\t\t{\n");
          for (j = 0; j < fsm->max_branches_per_trans; j++)
          {
              fprintf (f, "\t\t\t\t{angfsm_BRANCH_%s_NB, angfsm_STATE_%s_NB}",
                       fsm->name,
                       fsm->name);
              if (j != fsm->max_branches_per_trans - 1)
                  fprintf (f, ",");
              fprintf (f, "\n");
          }
          fprintf (f, "\t\t\t}\n");
          if (k != fsm->max_events_per_states - 1)
             fprintf (f, "\t\t},\n");
          else
              fprintf (f, "\t\t}\n");
      }

      if (i == fsm->state_nb - 1)
          fprintf (f, "\t}\n");
      else
          fprintf (f, "\t},\n");
   }
   fprintf (f, "};\n\n");

   /* Transition callback. */
   fprintf (f, "void (*angfsm_%s_trans_callback) (int state, int event, int output_branch, int branch);\n", fsm->name);

   /* Gen active states array. */
   fprintf (f, "angfsm_%s_state_t angfsm_%s_active_states[angfsm_%s_max_active_states];\n\n",
            fsm->name,
            fsm->name,
            fsm->name);

   /* Gen read transition from event and state. */
   fprintf (f, "inline angfsm_%s_trans_t* angfsm_%s_read_trans "
            "(angfsm_%s_event_t event, angfsm_%s_state_t state)\n{\n",
            fsm->name,
            fsm->name,
            fsm->name,
            fsm->name);
   fprintf (f, "\tint i;\n");
   fprintf (f, "\tangfsm_%s_trans_t* t;\n", fsm->name);
   fprintf (f, "\tfor (i = 0; i < angfsm_%s_max_events_per_states; i++)\n\t{\n",
            fsm->name);
   fprintf (f, "\t\tt = (angfsm_%s_trans_t *) (&angfsm_%s_trans_table[state][i]);\n",
            fsm->name,
            fsm->name);
   fprintf (f, "\t\tif (t->func == (angfsm_%s_func_t) 0)\n\t\t\tbreak;\n", fsm->name);
   fprintf (f, "\t\tif (t->event == event)\n");
   fprintf (f, "\t\t\treturn t;\n");
   fprintf (f, "\t}\n\treturn (angfsm_%s_trans_t*) 0;\n}\n\n", fsm->name);

   /* Gen initialization function. */
   sc = fsm->starters;
   i = 0;
   fprintf (f, "void\nangfsm_%s_init ()\n{\n", fsm->name);
   while (sc != NULL)
   {
      fprintf (f, "\tangfsm_%s_active_states[%u] = angfsm_STATE_%s_%s;\n",
               fsm->name,
               i,
               fsm->name,
               sc->state.var_name);
      if (fsm->timeouts != NULL)
      {
         toc = fsm->timeouts;
         while (toc != NULL)
         {
            if (toc->timeout.trans->state->code == sc->state.code)
            {
               fprintf (f, "\tangfsm_%s_timeout_counters[%u] = %u;\n",
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
   /* Set transition callback to NULL. */
   fprintf (f, "\tangfsm_%s_trans_callback = (typeof (angfsm_%s_trans_callback)) 0;\n", fsm->name, fsm->name);
   fprintf (f, "}\n\n");

   /* Gen handle function. */
   fprintf (f, "int\nangfsm_%s_handle (angfsm_%s_event_t e)\n{\n",
            fsm->name,
            fsm->name);
   fprintf (f, "\tuint16_t i, j;\n");
   fprintf (f, "\tint handled = 0;\n");
   fprintf (f, "\tangfsm_%s_branch_t b;\n", fsm->name);
   fprintf (f, "\tfor (i = 0; i < angfsm_%s_max_active_states; i++)\n\t{\n",
            fsm->name);
   fprintf (f, "\t\tangfsm_%s_trans_t *trans = angfsm_%s_read_trans (e, angfsm_%s_active_states[i]);\n",
            fsm->name,
            fsm->name,
            fsm->name);
   fprintf (f, "\t\tif (trans)\n");
   fprintf (f, "\t\t{\n");
   fprintf (f, "\t\t\tangfsm_%s_state_t s = angfsm_STATE_%s_NB;\n", fsm->name, fsm->name);
   fprintf (f, "\t\t\tif (trans->branches[0].branch == angfsm_BRANCH_%s_NB)\n\t\t\t{\n", fsm->name);
   fprintf (f, "\t\t\t\ttrans->func ();\n");
   fprintf (f, "\t\t\t\ts = trans->branches[0].state;\n");
   fprintf (f, "\t\t\t\tif (angfsm_%s_trans_callback)\n", fsm->name);
   fprintf (f, "\t\t\t\t\tangfsm_%s_trans_callback(angfsm_%s_active_states[i], e, s, -1);\n", fsm->name, fsm->name);
   fprintf (f, "\t\t\t}\n\t\t\telse\n\t\t\t{\n");
   fprintf (f, "\t\t\t\tb = ((angfsm_%s_func_branches_t) trans->func) ();\n", fsm->name);
   fprintf (f, "\t\t\t\tfor (j = 0; j < angfsm_%s_max_branches_per_trans; j++)\n", fsm->name);
   fprintf (f, "\t\t\t\t\tif (trans->branches[j].branch == b)\n");
   fprintf (f, "\t\t\t\t\t\ts = trans->branches[j].state;\n");
   fprintf (f, "\t\t\t\tif (angfsm_%s_trans_callback)\n", fsm->name);
   fprintf (f, "\t\t\t\t\tangfsm_%s_trans_callback(angfsm_%s_active_states[i], e, s, b);\n", fsm->name, fsm->name);
   fprintf (f, "\t\t\t}\n");
   fprintf (f, "\t\t\tif (s != angfsm_STATE_%s_NB)\n", fsm->name);
   fprintf (f, "\t\t\t\tangfsm_%s_active_states[i] = s;\n", fsm->name);
   fprintf (f, "\t\t\telse\n");
   fprintf (f, "\t\t\t\t{;} //XXX show some error ?\n");
   fprintf (f, "\t\t\thandled = 1;\n");
   if (fsm->timeouts != NULL)
   {
      fprintf (f, "\t\t\tangfsm_%s_timeout_counters[i] = "
               "angfsm_%s_timeout_values[angfsm_%s_active_states[i]];\n",
               fsm->name,
               fsm->name,
               fsm->name);
   }
   fprintf (f, "\t\t}\n");
   fprintf (f, "\t}\n");
   fprintf (f, "\treturn handled;\n");
   fprintf (f, "}\n\n");

   /* Gen can handle function. */
   fprintf (f, "uint16_t\nangfsm_%s_can_handle (angfsm_%s_event_t e)\n{\n",
            fsm->name,
            fsm->name);
   fprintf (f, "\tuint16_t i;\n");
   fprintf (f, "\tfor (i = 0; i < angfsm_%s_max_active_states; i++)\n",
            fsm->name);
   fprintf (f, "\t\tif (angfsm_%s_read_trans (e, angfsm_%s_active_states[i]))\n",
            fsm->name,
            fsm->name);
   fprintf (f, "\t\t\treturn 1;\n");
   fprintf (f, "\treturn 0;\n");
   fprintf (f, "}\n\n");

   if (fsm->timeouts != NULL)
   {
      /* Gen timeout counters array. */
      fprintf (f, "int32_t angfsm_%s_timeout_counters[angfsm_%s_max_active_states];\n",
               fsm->name,
               fsm->name);

      /* Gen timeout values array. */
      fprintf (f, "int32_t angfsm_%s_timeout_values[angfsm_STATE_%s_NB] =\n{\n",
               fsm->name,
               fsm->name);
      int value;
      for (i = 0; i < fsm->state_nb; i++)
      {
         value = -1;
         s = angfsm_build_get_state_by_code (fsm, i);
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
      fprintf (f, "angfsm_%s_event_t angfsm_%s_timeout_events[angfsm_STATE_%s_NB] =\n{\n",
               fsm->name,
               fsm->name,
               fsm->name);
      for (i = 0; i < fsm->state_nb; i++)
      {
         value = -1;
         s = angfsm_build_get_state_by_code (fsm, i);
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
            fprintf (f, "\t(angfsm_%s_event_t) angfsm_STATE_%s_NB", fsm->name, fsm->name);
         else
            fprintf (f, "\t(angfsm_%s_event_t) %u", fsm->name, value);

         if (i != fsm->state_nb - 1)
            fprintf (f, ",");
         fprintf (f, "\n");
      }
      fprintf (f, "};\n\n");

      /* Gen handle timeout function. */
      fprintf (f, "int\nangfsm_%s_handle_timeout ()\n{\n",
               fsm->name);
      fprintf (f, "\tuint16_t i;\n");
      fprintf (f, "\tint out = 0;\n");
      fprintf (f, "\tfor (i = 0; i < angfsm_%s_max_active_states; i++)\n\t{\n",
               fsm->name);
      fprintf (f, "\t\tif (angfsm_%s_timeout_counters[i] > 0)\n\t\t{\n",
               fsm->name);
      fprintf (f, "\t\t\tangfsm_%s_timeout_counters[i]--;\n",
               fsm->name);
      fprintf (f, "\t\t\tif (angfsm_%s_timeout_counters[i] == 0)\n\t\t\t{\n",
               fsm->name);
      fprintf (f, "\t\t\t\tangfsm_%s_handle (angfsm_%s_timeout_events[angfsm_%s_active_states[i]]);\n",
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
   free (fn);
}

void
angfsm_build_gen_opti_avr_h (angfsm_build_t *fsm, angfsm_build_arch_t arch)
{
   assert (arch < ANGFSM_BUILD_ARCH_NB);
   angfsm_build_state_chain_t *sc;
   angfsm_build_event_chain_t *ec;
   angfsm_build_trans_chain_t *tc;
   angfsm_build_chain_t *all_fsm;
   angfsm_build_state_t *s;
   angfsm_build_event_t *e;

   uint i, j;
   uint embedded_strings = fsm->options.embedded_strings;

   /* Open file. */
   char *fn = (char*) malloc ((strlen (fsm->name)
                               + strlen ("angfsm_gen__.h")
			       + strlen (angfsm_build_arch_name[arch])
			       + 1) * sizeof (char));
   sprintf (fn, "angfsm_gen_%s_%s.h", angfsm_build_arch_name[arch], fsm->name);
   FILE *f = fopen (fn, "w");

   /* Introduction. */
   fprintf (f, "/* This file has been generated, do not edit. */\n\n");
   fprintf (f, "#ifndef _angfsm_%s_\n", fsm->name);
   fprintf (f, "#define _angfsm_%s_\n\n", fsm->name);
   fprintf (f, "#include <avr/pgmspace.h>\n");
   fprintf (f, "#include <inttypes.h>\n\n");

   /* Include all other fsm headers. */
   all_fsm = angfsm_build_all_fsm;
   while (all_fsm != NULL)
   {
      fprintf (f, "#include \"angfsm_gen_%s_%s.h\"\n",
	       angfsm_build_arch_name[arch],
	       all_fsm->fsm->name);
      all_fsm = all_fsm->next;
   }

   /* Gen max active states define */
   fprintf (f, "#define angfsm_%s_max_active_states %u\n",
            fsm->name,
            fsm->max_active_states);

   /* Gen max number of events per states. */
   fprintf (f, "#define angfsm_%s_max_events_per_states %u\n",
            fsm->name,
            fsm->max_events_per_states);

   /* Gen max number of branches per transitions. */
   fprintf (f, "#define angfsm_%s_max_branches_per_trans %u\n",
            fsm->name,
            fsm->max_branches_per_trans);

   /* Gen state enum. */
   fprintf (f, "typedef enum\n{\n");
   sc = fsm->states;
   while (sc != NULL)
   {
      fprintf (f, "\tangfsm_STATE_%s_%s = %u,\n", fsm->name, sc->state.var_name, sc->state.code);
      sc = sc->next;
   }
   fprintf (f, "\tangfsm_STATE_%s_NB = %u\n", fsm->name, fsm->state_nb);
   fprintf (f, "} angfsm_%s_state_t;\n\n", fsm->name);

   /* Gen transition callback reference. */
   fprintf (f, "extern void (*angfsm_%s_trans_callback) (int state, int event, int output_branch, int branch);\n", fsm->name);

   /* Gen event enum. */
   fprintf (f, "typedef enum\n{\n");
   ec = fsm->events;
   while (ec != NULL)
   {
      fprintf (f, "\tangfsm_EVENT_%s_%s = %u,\n", fsm->name, ec->event.var_name, ec->event.code);
      ec = ec->next;
   }
   fprintf (f, "\tangfsm_EVENT_%s_NB = %u\n", fsm->name, fsm->event_nb);
   fprintf (f, "} angfsm_%s_event_t;\n\n", fsm->name);

   /* Gen branches unique name enum. */
   fprintf (f, "typedef enum\n{\n");
   if (fsm->u_branch_nb > 0)
   {
      for (i = 0; i < fsm->u_branch_nb; i++)
          fprintf (f, "\tangfsm_BRANCH_%s_%s = %u,\n",
                   fsm->name,
                   fsm->u_branch_name[i],
                   i);
   }
   fprintf (f, "\tangfsm_BRANCH_%s_NB = %u\n", fsm->name, fsm->u_branch_nb);
   fprintf (f, "} angfsm_%s_branch_t;\n\n", fsm->name);

   /* Gen strings. */
   if (embedded_strings)
   {
      /* Gen state strings header. */
      sc = fsm->states;
      while (sc != NULL)
      {
         fprintf (f, "extern prog_char angfsm_%s_state_str_%s[%u] PROGMEM;\n",
                  fsm->name,
                  sc->state.var_name,
                  strlen (sc->state.var_name) + 1);
         sc = sc->next;
      }
      fprintf (f, "extern const char *angfsm_%s_state_str[%u] PROGMEM;\n\n",
               fsm->name,
               fsm->state_nb);

      /* Gen event strings header. */
      ec = fsm->events;
      while (ec != NULL)
      {
         fprintf (f, "extern prog_char angfsm_%s_event_str_%s[%u] PROGMEM;\n",
                  fsm->name,
                  ec->event.var_name,
                  strlen (ec->event.var_name) + 1);
         ec = ec->next;
      }
      fprintf (f, "extern const char *angfsm_%s_event_str[%u] PROGMEM;\n\n",
               fsm->name,
               fsm->event_nb);

      /* Gen branch strings header. */
      for (i = 0; i < fsm->u_branch_nb; i++)
      {
         fprintf (f, "extern prog_char angfsm_%s_branch_str_%s[%u] PROGMEM;\n",
                  fsm->name,
                  fsm->u_branch_name[i],
                  strlen (fsm->u_branch_name[i]) + 1);
      }
      fprintf (f, "extern const char *angfsm_%s_branch_str[%u] PROGMEM;\n\n",
               fsm->name,
               fsm->u_branch_nb);

      /* Create a RAM string able to store event or state string. */
      j = 0;
      for (i = 0; i < fsm->event_nb; i++)
      {
         e = angfsm_build_get_event_by_code (fsm, i);
         if (strlen (e->var_name) > j)
            j = strlen (e->var_name);
      }
      for (i = 0; i < fsm->state_nb; i++)
      {
         s = angfsm_build_get_state_by_code (fsm, i);
         if (strlen (s->var_name) > j)
            j = strlen (s->var_name);
      }
      for (i = 0; i < fsm->u_branch_nb; i++)
      {
         if (strlen (fsm->u_branch_name[i]) > j)
            j = strlen (fsm->u_branch_name[i]);
      }
      fprintf (f, "extern char angfsm_%s_str_buff[%u];\n", fsm->name, j + 1);

      /* Convert an event in string. */
      fprintf (f, "char *\nangfsm_%s_get_event_str (angfsm_%s_event_t e);\n", fsm->name, fsm->name);
      /* Convert a state in string. */
      fprintf (f, "char *\nangfsm_%s_get_state_str (angfsm_%s_state_t s);\n", fsm->name, fsm->name);
      /* Convert a branch in string. */
      fprintf (f, "char *\nangfsm_%s_get_branch_str (angfsm_%s_branch_t s);\n", fsm->name, fsm->name);
   }
   else
   {
      /* Disable string macros. */
      fprintf (f, "#undef ANGFSM_STATE_STR\n#define ANGFSM_STATE_STR(s) ((char *)0)\n");
      fprintf (f, "#undef ANGFSM_EVENT_STR\n#define ANGFSM_EVENT_STR(e) ((char *)0)\n");
      fprintf (f, "#undef ANGFSM_BRANCH_STR\n#define ANGFSM_BRANCH_STR(b) ((char *)0)\n");
   }

   /* Gen function headers. */
   tc = fsm->trans;
   while (tc != NULL)
   {
      assert (tc->trans.output_branches);
      /* Mono branch */
      if (tc->trans.output_branches->next==NULL)
         fprintf (f, "void angfsm_%s_trans_func_%s_%s ();\n",
                  fsm->name,
                  tc->trans.state->var_name,
                  tc->trans.event->var_name);
      else
         fprintf (f, "angfsm_%s_branch_t angfsm_%s_trans_func_%s_%s ();\n",
                  fsm->name,
                  fsm->name,
                  tc->trans.state->var_name,
                  tc->trans.event->var_name);
      tc = tc->next;
   }
   fprintf (f, "\n");

   /* Declare function type. */
   fprintf (f, "typedef void (*angfsm_%s_func_t)();\n", fsm->name);
   fprintf (f, "typedef angfsm_%s_branch_t (*angfsm_%s_func_branches_t)();\n\n",
            fsm->name, fsm->name);

   /* Gen branch structure. */
   fprintf (f, "typedef struct\n{\n");
   fprintf (f, "\tangfsm_%s_branch_t branch;\n", fsm->name);
   fprintf (f, "\tangfsm_%s_state_t state;\n", fsm->name);
   fprintf (f, "} angfsm_%s_next_t;\n\n", fsm->name);

   /* Gen transition structure. */
   fprintf (f, "typedef struct\n{\n");
   fprintf (f, "\tangfsm_%s_event_t event;\n", fsm->name);
   fprintf (f, "\tangfsm_%s_func_t func;\n", fsm->name);
   fprintf (f, "\tangfsm_%s_next_t branches[angfsm_%s_max_branches_per_trans];\n",
            fsm->name,
            fsm->name);
   fprintf (f, "} angfsm_%s_trans_t;\n\n", fsm->name);

   /* Gen transition table. */
   fprintf (f, "extern const angfsm_%s_trans_t PROGMEM angfsm_%s_trans_table[angfsm_STATE_%s_NB][angfsm_%s_max_events_per_states];\n\n",
            fsm->name,
            fsm->name,
            fsm->name,
            fsm->name);

   /* Gen transition living in RAM. */
   fprintf (f, "extern angfsm_%s_trans_t angfsm_%s_trans_tmp;\n\n", fsm->name, fsm->name);

   /* Gen read function for trans table. */
   fprintf (f, "inline angfsm_%s_trans_t* angfsm_%s_read_trans (angfsm_%s_event_t event, angfsm_%s_state_t state);\n\n",
            fsm->name,
            fsm->name,
            fsm->name,
            fsm->name);

   /* Gen active states array. */
   fprintf (f, "extern angfsm_%s_state_t angfsm_%s_active_states[angfsm_%s_max_active_states];\n\n",
            fsm->name,
            fsm->name,
            fsm->name);

   /* Gen initialization function. */
   sc = fsm->starters;
   i = 0;
   fprintf (f, "void\nangfsm_%s_init () __attribute__ ((constructor));\n\n",
            fsm->name);

   /* Gen handle function. */
   fprintf (f, "int\nangfsm_%s_handle (angfsm_%s_event_t e);\n",
            fsm->name,
            fsm->name);

   /* Gen can handle function. */
   fprintf (f, "uint16_t\nangfsm_%s_can_handle (angfsm_%s_event_t e);\n\n",
            fsm->name,
            fsm->name);

   if (fsm->timeouts != NULL)
   {
      /* Gen handle timeout function. */
      fprintf (f, "int\nangfsm_%s_handle_timeout ();\n",
               fsm->name);

      /* Gen timeout values. */
      fprintf (f, "extern int32_t angfsm_%s_timeout_values[angfsm_STATE_%s_NB];\n",
               fsm->name,
               fsm->name);

      /* Gen timeout corresponding events. */
      fprintf (f, "extern angfsm_%s_event_t angfsm_%s_timeout_events[angfsm_STATE_%s_NB];\n",
               fsm->name,
               fsm->name,
               fsm->name);

      /* Gen timeout counters array. */
      fprintf (f, "extern int32_t angfsm_%s_timeout_counters[angfsm_%s_max_active_states];\n\n",
               fsm->name,
               fsm->name);
   }

   /* Conclusion. */
   fprintf (f, "#endif /* #ifndef _angfsm_%s_ */", fsm->name);

   /* Close file. */
   fclose (f);
   free (fn);
}

void
angfsm_build_gen_opti_avr_c (angfsm_build_t *fsm, angfsm_build_arch_t arch)
{
   assert (fsm);
   assert (arch < ANGFSM_BUILD_ARCH_NB);
   angfsm_build_state_chain_t *sc;
   angfsm_build_event_chain_t *ec;
   angfsm_build_trans_chain_t *tc;
   angfsm_build_timeout_chain_t *toc;
   angfsm_build_branch_chain_t *bc;
   angfsm_build_state_t *s;
   angfsm_build_event_t *e;
   uint i, j, k, count;
   uint embedded_strings = fsm->options.embedded_strings;

   /* Open file. */
   char *fn = (char*) malloc ((strlen (fsm->name)
                               + strlen ("angfsm_gen__.c")
			       + strlen (angfsm_build_arch_name[arch])
			       + 1) * sizeof (char));
   sprintf (fn, "angfsm_gen_%s_%s.c", angfsm_build_arch_name[arch], fsm->name);
   FILE *f = fopen (fn, "w");

   /* Introduction. */
   fprintf (f, "/* This file has been generated, do not edit. */\n\n");
   fprintf (f, "#include \"angfsm_gen_%s_%s.h\"\n\n",
	    angfsm_build_arch_name[arch],
	    fsm->name);

   /* Gen strings if configured. */
   if (embedded_strings)
   {
      /* Gen state strings. */
      sc = fsm->states;
      while (sc != NULL)
      {
         fprintf (f, "prog_char angfsm_%s_state_str_%s[] PROGMEM = \"%s\";\n",
                  fsm->name,
                  sc->state.var_name,
                  sc->state.var_name);
         sc = sc->next;
      }
      fprintf (f, "const char *angfsm_%s_state_str[] PROGMEM =\n{\n", fsm->name);
      for (i = 0; i < fsm->state_nb; i++)
      {
         s = angfsm_build_get_state_by_code (fsm, i);
         fprintf (f, "\tangfsm_%s_state_str_%s", fsm->name, s->var_name);
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
         fprintf (f, "prog_char angfsm_%s_event_str_%s[] PROGMEM = \"%s\";\n",
                  fsm->name,
                  ec->event.var_name,
                  ec->event.var_name);
         ec = ec->next;
      }
      fprintf (f, "const char *angfsm_%s_event_str[] PROGMEM =\n{\n", fsm->name);
      for (i = 0; i < fsm->event_nb; i++)
      {
         e = angfsm_build_get_event_by_code (fsm, i);
         fprintf (f, "\tangfsm_%s_event_str_%s", fsm->name, e->var_name);
         if (i == fsm->event_nb - 1)
            fprintf (f, "\n");
         else
            fprintf (f, ",\n");
      }
      fprintf (f, "};\n\n");

      /* Gen branch strings. */
      for (i = 0; i < fsm->u_branch_nb; i++)
      {
         fprintf (f, "prog_char angfsm_%s_branch_str_%s[] PROGMEM = \"%s\";\n",
                  fsm->name,
                  fsm->u_branch_name[i],
                  fsm->u_branch_name[i]);
      }
      fprintf (f, "const char *angfsm_%s_branch_str[] PROGMEM =\n{\n", fsm->name);
      for (i = 0; i < fsm->u_branch_nb; i++)
      {
         fprintf (f, "\tangfsm_%s_branch_str_%s", fsm->name, fsm->u_branch_name[i]);
         if (i == fsm->u_branch_nb - 1)
            fprintf (f, "\n");
         else
            fprintf (f, ",\n");
      }
      fprintf (f, "};\n\n");

      /* Create a RAM string able to store event or state string. */
      j = 0;
      for (i = 0; i < fsm->event_nb; i++)
      {
         e = angfsm_build_get_event_by_code (fsm, i);
         if (strlen (e->var_name) > j)
            j = strlen (e->var_name);
      }
      for (i = 0; i < fsm->state_nb; i++)
      {
         s = angfsm_build_get_state_by_code (fsm, i);
         if (strlen (s->var_name) > j)
            j = strlen (s->var_name);
      }
      for (i = 0; i < fsm->u_branch_nb; i++)
      {
         if (strlen (fsm->u_branch_name[i]) > j)
            j = strlen (fsm->u_branch_name[i]);
      }
      fprintf (f, "char angfsm_%s_str_buff[%u];\n", fsm->name, j + 1);

      /* Generate string read functions. */
      /* Convert an event enum in string. */
      fprintf (f, "char *\nangfsm_%s_get_event_str (angfsm_%s_event_t e)\n{\n", fsm->name, fsm->name);
      fprintf (f, "\treturn strcpy_P (angfsm_%s_str_buff, (PGM_P) pgm_read_word (&(angfsm_%s_event_str[e])));\n", fsm->name, fsm->name);
      fprintf (f, "}\n\n");
      /* Convert a state enum in string. */
      fprintf (f, "char *\nangfsm_%s_get_state_str (angfsm_%s_state_t s)\n{\n", fsm->name, fsm->name);
      fprintf (f, "\treturn strcpy_P (angfsm_%s_str_buff, (PGM_P) pgm_read_word (&(angfsm_%s_state_str[s])));\n", fsm->name, fsm->name);
      fprintf (f, "}\n\n");
      /* Convert a branch enum in string. */
      fprintf (f, "char *\nangfsm_%s_get_branch_str (angfsm_%s_branch_t b)\n{\n", fsm->name, fsm->name);
      fprintf (f, "\treturn strcpy_P (angfsm_%s_str_buff, (PGM_P) pgm_read_word (&(angfsm_%s_branch_str[b])));\n", fsm->name, fsm->name);
      fprintf (f, "}\n\n");
   }

   /* Gen transition table. */
   fprintf (f, "const angfsm_%s_trans_t PROGMEM angfsm_%s_trans_table[angfsm_STATE_%s_NB][angfsm_%s_max_events_per_states] = \n{\n",
            fsm->name,
            fsm->name,
            fsm->name,
            fsm->name);
   for (i = 0; i < fsm->state_nb; i++)
   {
      count = 0;
      fprintf (f, "\t{\n");
      tc = fsm->trans;
      while (tc != NULL)
      {
          if (tc->trans.state->code == i)
          {
             fprintf (f, "\t\t{\n");
             fprintf (f, "\t\t\tangfsm_EVENT_%s_%s,\n",
                      fsm->name,
                      tc->trans.event->var_name);
             fprintf (f, "\t\t\t(angfsm_%s_func_t) &angfsm_%s_trans_func_%s_%s,\n",
                      fsm->name,
                      fsm->name,
                      tc->trans.state->var_name,
                      tc->trans.event->var_name);
             /* Gen branche array */
             fprintf (f, "\t\t\t{\n");
             j = 0;
             bc = tc->trans.output_branches;
             while (bc != NULL)
             {
                fprintf (f, "\t\t\t\t{angfsm_BRANCH_%s_%s, angfsm_STATE_%s_%s}",
                         fsm->name,
                         (bc->name ? bc->name : "NB"),
                         fsm->name,
                         bc->state->var_name);
                if (j != fsm->max_branches_per_trans - 1)
                    fprintf (f, ", ");
                fprintf (f, "\n");
                j++;
                bc = bc->next;
             }
             /* Fill blank branches. */
             for (k = j; k < fsm->max_branches_per_trans; k++)
             {
                 fprintf (f, "\t\t\t\t{angfsm_BRANCH_%s_NB, angfsm_STATE_%s_NB}",
                          fsm->name,
                          fsm->name);
                 if (k != fsm->max_branches_per_trans - 1)
                    fprintf (f, ",");
                 fprintf (f, "\n");
             }

             fprintf (f, "\t\t\t}\n");
             if (tc->next != NULL || count != fsm->max_events_per_states)
                 fprintf (f, "\t\t},\n");
             else
                 fprintf (f, "\t\t}\n");
             count++;
          }
          tc = tc->next;
      }
      /* Fill blanch transitions. */
      for (k = count; k < fsm->max_events_per_states; k++)
      {
          fprintf (f, "\t\t{\n");
          fprintf (f, "\t\t\tangfsm_EVENT_%s_NB,\n",fsm->name);
          fprintf (f, "\t\t\t(angfsm_%s_func_t) 0,\n", fsm->name);
          fprintf (f, "\t\t\t{\n");
          for (j = 0; j < fsm->max_branches_per_trans; j++)
          {
              fprintf (f, "\t\t\t\t{angfsm_BRANCH_%s_NB, angfsm_STATE_%s_NB}",
                       fsm->name,
                       fsm->name);
              if (j != fsm->max_branches_per_trans - 1)
                  fprintf (f, ",");
              fprintf (f, "\n");
          }
          fprintf (f, "\t\t\t}\n");
          if (k != fsm->max_events_per_states - 1)
             fprintf (f, "\t\t},\n");
          else
              fprintf (f, "\t\t}\n");
      }

      if (i == fsm->state_nb - 1)
          fprintf (f, "\t}\n");
      else
          fprintf (f, "\t},\n");
   }
   fprintf (f, "};\n\n");

   /* Transition callback. */
   fprintf (f, "void (*angfsm_%s_trans_callback) (int state, int event, int output_branch, int branch);\n", fsm->name);

   /* Gen active states array. */
   fprintf (f, "angfsm_%s_state_t angfsm_%s_active_states[angfsm_%s_max_active_states];\n\n",
            fsm->name,
            fsm->name,
            fsm->name);

   /* Gen transition living in RAM. */
   fprintf (f, "angfsm_%s_trans_t angfsm_%s_trans_tmp;\n", fsm->name, fsm->name);

   /* Gen read transition from event and state. */
   fprintf (f, "inline angfsm_%s_trans_t* angfsm_%s_read_trans "
            "(angfsm_%s_event_t event, angfsm_%s_state_t state)\n{\n",
            fsm->name,
            fsm->name,
            fsm->name,
            fsm->name);
   fprintf (f, "\tint i;\n");
   fprintf (f, "\tangfsm_%s_trans_t* t;\n", fsm->name);
   fprintf (f, "\tfor (i = 0; i < angfsm_%s_max_events_per_states; i++)\n\t{\n",
            fsm->name);
   fprintf (f, "\t\tt = (angfsm_%s_trans_t *) memcpy_P ((void *) (&angfsm_%s_trans_tmp), (PGM_VOID_P) &(angfsm_%s_trans_table[state][i]), sizeof (angfsm_%s_trans_t));\n",
            fsm->name,
            fsm->name,
            fsm->name,
            fsm->name);
   fprintf (f, "\t\tif (t->func == (angfsm_%s_func_t) 0)\n\t\t\tbreak;\n", fsm->name);
   fprintf (f, "\t\tif (t->event == event)\n");
   fprintf (f, "\t\t\treturn t;\n");
   fprintf (f, "\t}\n\treturn (angfsm_%s_trans_t*) 0;\n}\n\n", fsm->name);

   /* Gen initialization function. */
   sc = fsm->starters;
   i = 0;
   fprintf (f, "void\nangfsm_%s_init ()\n{\n", fsm->name);
   while (sc != NULL)
   {
      fprintf (f, "\tangfsm_%s_active_states[%u] = angfsm_STATE_%s_%s;\n",
               fsm->name,
               i,
               fsm->name,
               sc->state.var_name);
      if (fsm->timeouts != NULL)
      {
         toc = fsm->timeouts;
         while (toc != NULL)
         {
            if (toc->timeout.trans->state->code == sc->state.code)
            {
               fprintf (f, "\tangfsm_%s_timeout_counters[%u] = %u;\n",
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
   /* Set transition callback to NULL. */
   fprintf (f, "\tangfsm_%s_trans_callback = (typeof (angfsm_%s_trans_callback)) 0;\n", fsm->name, fsm->name);
   fprintf (f, "}\n\n");

   /* Gen handle function. */
   fprintf (f, "int\nangfsm_%s_handle (angfsm_%s_event_t e)\n{\n",
            fsm->name,
            fsm->name);
   fprintf (f, "\tuint16_t i, j;\n");
   fprintf (f, "\tint handled = 0;\n");
   fprintf (f, "\tangfsm_%s_branch_t b;\n", fsm->name);
   fprintf (f, "\tfor (i = 0; i < angfsm_%s_max_active_states; i++)\n\t{\n",
            fsm->name);
   fprintf (f, "\t\tangfsm_%s_trans_t *trans = angfsm_%s_read_trans (e, angfsm_%s_active_states[i]);\n",
            fsm->name,
            fsm->name,
            fsm->name);
   fprintf (f, "\t\tif (trans)\n");
   fprintf (f, "\t\t{\n");
   fprintf (f, "\t\t\tangfsm_%s_state_t s = angfsm_STATE_%s_NB;\n", fsm->name, fsm->name);
   fprintf (f, "\t\t\tif (trans->branches[0].branch == angfsm_BRANCH_%s_NB)\n\t\t\t{\n", fsm->name);
   fprintf (f, "\t\t\t\ttrans->func ();\n");
   fprintf (f, "\t\t\t\ts = trans->branches[0].state;\n");
   fprintf (f, "\t\t\t\tif (angfsm_%s_trans_callback)\n", fsm->name);
   fprintf (f, "\t\t\t\t\tangfsm_%s_trans_callback(angfsm_%s_active_states[i], e, s, -1);\n", fsm->name, fsm->name);
   fprintf (f, "\t\t\t}\n\t\t\telse\n\t\t\t{\n");
   fprintf (f, "\t\t\t\tb = ((angfsm_%s_func_branches_t) trans->func) ();\n", fsm->name);
   fprintf (f, "\t\t\t\tfor (j = 0; j < angfsm_%s_max_branches_per_trans; j++)\n", fsm->name);
   fprintf (f, "\t\t\t\t\tif (trans->branches[j].branch == b)\n");
   fprintf (f, "\t\t\t\t\t\ts = trans->branches[j].state;\n");
   fprintf (f, "\t\t\t\tif (angfsm_%s_trans_callback)\n", fsm->name);
   fprintf (f, "\t\t\t\t\tangfsm_%s_trans_callback(angfsm_%s_active_states[i], e, s, b);\n", fsm->name, fsm->name);
   fprintf (f, "\t\t\t}\n");
   fprintf (f, "\t\t\tif (s != angfsm_STATE_%s_NB)\n", fsm->name);
   fprintf (f, "\t\t\t\tangfsm_%s_active_states[i] = s;\n", fsm->name);
   fprintf (f, "\t\t\telse\n");
   fprintf (f, "\t\t\t\t{;} //XXX show some error ?\n");
   fprintf (f, "\t\t\thandled = 1;\n");
   if (fsm->timeouts != NULL)
   {
      fprintf (f, "\t\t\tangfsm_%s_timeout_counters[i] = "
               "angfsm_%s_timeout_values[angfsm_%s_active_states[i]];\n",
               fsm->name,
               fsm->name,
               fsm->name);
   }
   fprintf (f, "\t\t}\n");
   fprintf (f, "\t}\n");
   fprintf (f, "\treturn handled;\n");
   fprintf (f, "}\n\n");

   /* Gen can handle function. */
   fprintf (f, "uint16_t\nangfsm_%s_can_handle (angfsm_%s_event_t e)\n{\n",
            fsm->name,
            fsm->name);
   fprintf (f, "\tuint16_t i;\n");
   fprintf (f, "\tfor (i = 0; i < angfsm_%s_max_active_states; i++)\n",
            fsm->name);
   fprintf (f, "\t\tif (angfsm_%s_read_trans (e, angfsm_%s_active_states[i]))\n",
            fsm->name,
            fsm->name);
   fprintf (f, "\t\t\treturn 1;\n");
   fprintf (f, "\treturn 0;\n");
   fprintf (f, "}\n\n");

   if (fsm->timeouts != NULL)
   {
      /* Gen timeout counters array. */
      fprintf (f, "int32_t angfsm_%s_timeout_counters[angfsm_%s_max_active_states];\n",
               fsm->name,
               fsm->name);

      /* Gen timeout values array. */
      fprintf (f, "int32_t angfsm_%s_timeout_values[angfsm_STATE_%s_NB] =\n{\n",
               fsm->name,
               fsm->name);
      int value;
      for (i = 0; i < fsm->state_nb; i++)
      {
         value = -1;
         s = angfsm_build_get_state_by_code (fsm, i);
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
      fprintf (f, "angfsm_%s_event_t angfsm_%s_timeout_events[angfsm_STATE_%s_NB] =\n{\n",
               fsm->name,
               fsm->name,
               fsm->name);
      for (i = 0; i < fsm->state_nb; i++)
      {
         value = -1;
         s = angfsm_build_get_state_by_code (fsm, i);
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
            fprintf (f, "\t(angfsm_%s_event_t) angfsm_STATE_%s_NB", fsm->name, fsm->name);
         else
            fprintf (f, "\t(angfsm_%s_event_t) %u", fsm->name, value);

         if (i != fsm->state_nb - 1)
            fprintf (f, ",");
         fprintf (f, "\n");
      }
      fprintf (f, "};\n\n");

      /* Gen handle timeout function. */
      fprintf (f, "int\nangfsm_%s_handle_timeout ()\n{\n",
               fsm->name);
      fprintf (f, "\tuint16_t i;\n");
      fprintf (f, "\tint out = 0;\n");
      fprintf (f, "\tfor (i = 0; i < angfsm_%s_max_active_states; i++)\n\t{\n",
               fsm->name);
      fprintf (f, "\t\tif (angfsm_%s_timeout_counters[i] > 0)\n\t\t{\n",
               fsm->name);
      fprintf (f, "\t\t\tangfsm_%s_timeout_counters[i]--;\n",
               fsm->name);
      fprintf (f, "\t\t\tif (angfsm_%s_timeout_counters[i] == 0)\n\t\t\t{\n",
               fsm->name);
      fprintf (f, "\t\t\t\tangfsm_%s_handle (angfsm_%s_timeout_events[angfsm_%s_active_states[i]]);\n",
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
   free (fn);
}

void
angfsm_build_gen_c (angfsm_build_t *fsm,
                    angfsm_build_arch_t arch)
{
   /* TODO: make a generic code generator. */
   assert (arch < ANGFSM_BUILD_ARCH_NB);

   switch (arch)
   {
      case ANGFSM_BUILD_ARCH_HOST:
         angfsm_build_gen_no_opti_c (fsm, arch);
         break;
      case ANGFSM_BUILD_ARCH_AVR:
         angfsm_build_gen_opti_avr_c (fsm, arch);
         break;
      case ANGFSM_BUILD_ARCH_ARM:
         angfsm_build_gen_no_opti_c (fsm, arch);
         break;
      default:
         assert (1);
   }
}

void
angfsm_build_gen_h (angfsm_build_t *fsm,
                    angfsm_build_arch_t arch)
{
   /* TODO: make a generic code generator. */
   assert (arch < ANGFSM_BUILD_ARCH_NB);

   switch (arch)
   {
      case ANGFSM_BUILD_ARCH_HOST:
         angfsm_build_gen_no_opti_h (fsm, arch);
         break;
      case ANGFSM_BUILD_ARCH_AVR:
         angfsm_build_gen_opti_avr_h (fsm, arch);
         break;
      case ANGFSM_BUILD_ARCH_ARM:
         angfsm_build_gen_no_opti_h (fsm, arch);
         break;
      default:
         assert (1);
   }
}

void
angfsm_build_gen (char *arch_string)
{
   angfsm_build_chain_t *curs = angfsm_build_all_fsm;
   angfsm_build_arch_t arch = ANGFSM_BUILD_ARCH_NB;
   uint i;
   for (i = 0; i < ANGFSM_BUILD_ARCH_NB; i++)
      if (strcmp (arch_string, angfsm_build_arch_name[i]) == 0)
      {
         arch = (angfsm_build_arch_t) i;
         break;
      }
   if (arch == ANGFSM_BUILD_ARCH_NB)
   {
      fprintf (stderr, "Warning, architecture didn't match for \"%s\".\n",
               arch_string);
      return;
   }

   while (curs != NULL)
   {
      angfsm_build_gen_h (curs->fsm, arch);
      angfsm_build_gen_c (curs->fsm, arch);
      curs = curs->next;
   }

}

void
angfsm_build_free (angfsm_build_t *fsm)
{
   angfsm_build_state_chain_t *sc;
   angfsm_build_state_chain_t *sc_tmp;
   angfsm_build_event_chain_t *ec;
   angfsm_build_event_chain_t *ec_tmp;
   angfsm_build_trans_chain_t *tc;
   angfsm_build_trans_chain_t *tc_tmp;
   angfsm_build_branch_chain_t *bc;
   angfsm_build_branch_chain_t *bc_tmp;
   angfsm_trans_func_chain_t *fc;
   angfsm_trans_func_chain_t *fc_tmp;
   angfsm_build_timeout_chain_t *toc;
   angfsm_build_timeout_chain_t *toc_tmp;
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

   /* Free unique branch names array. */
   /* Strings are already free in transitions. */
   if (fsm->u_branch_name)
      free (fsm->u_branch_name);

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
