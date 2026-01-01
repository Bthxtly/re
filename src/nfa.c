#include "state.c"
#include <stddef.h>
#include <stdlib.h>

char EPSILON = -1;
char ANY = -2;

typedef struct Edge {
  char symbol;
  State from;
  State to;
} Edge;

typedef struct NFA {
  State states_count;
  States *target_states;
  Edge *edges[MAX];
  unsigned int edges_count;
} NFA;

/* create a new NFA */
NFA *new_nfa() {
  NFA *nfa = (NFA *)malloc(sizeof(NFA));
  nfa->states_count = 0;
  nfa->target_states = NULL;
  nfa->edges_count = 0;
  return nfa;
}

/* create a new edge with given symbol, from, to */
Edge *new_edge(char symbol, State from, State to) {
  Edge *e = (Edge *)malloc(sizeof(Edge));
  e->symbol = symbol;
  e->from = from;
  e->to = to;
  return e;
}

/* set the states count of an NFA */
void set_states_count(NFA *nfa, State states_count) {
  nfa->states_count = states_count;
}

/* set the target states of an NFA */
void set_target_states(NFA *nfa, States *s) { nfa->target_states = s; }

/* add an edge to an NFA */
void push_edge(NFA *nfa, Edge *e) {
  nfa->edges[nfa->edges_count] = e;
  ++(nfa->edges_count);
}

/* print edges in the form of `from --symbol--> to` */
void print_edges(NFA *nfa) {
  for (size_t i = 0; i < nfa->edges_count; ++i) {
    Edge *e = nfa->edges[i];
    if (e->symbol == EPSILON)
      printf("%d --ε--> %d\n", e->from, e->to);
    else if (e->symbol == ANY)
      printf("%d --¤--> %d\n", e->from, e->to);
    else
      printf("%d --%c--> %d\n", e->from, e->symbol, e->to);
  }
}

/* free an NFA */
void free_nfa(NFA *nfa) {
  /* free edges */
  for (size_t i = 0; i < nfa->edges_count; ++i) {
    free(nfa->edges[i]);
  }
  /* free target states */
  if (nfa->target_states != NULL) {
    free(nfa->target_states);
  }
  /* free nfa */
  free(nfa);
  nfa = NULL;
}

/* return all states reachable with epsilon lables from the given states */
States *epsilon_closure(NFA *nfa, States *s) {
  States *new_s = new_states();

  /* add all original states to the closure first */
  for (size_t i = 0; i < s->len; ++i) {
    push_state(new_s, s->states[i]);
  }

  /* simulate the original states as a stack */
  while (s->len > 0) {
    --(s->len);
    State state = s->states[s->len];
    for (size_t i = 0; i < nfa->edges_count; ++i) {
      Edge *e = nfa->edges[i];
      if (e->symbol == EPSILON && e->from == state) {
        State next_state = e->to;
        if (!have_state(new_s, next_state)) {
          push_state(new_s, next_state);
          s->states[s->len] = next_state;
          ++(s->len);
        }
      }
    }
  }

  free(s);
  return new_s;
}

/* return all states reachable with given symbol from the given states */
States *move(NFA *nfa, States *s, char symbol) {
  States *new_s = new_states();
  for (size_t i = 0; i < s->len; ++i) {
    for (size_t j = 0; j < nfa->edges_count; ++j) {
      Edge *e = nfa->edges[j];
      if ((e->symbol == ANY || e->symbol == symbol) &&
          e->from == s->states[i]) {
        State next_state = e->to;
        if (!have_state(new_s, next_state)) {
          push_state(new_s, next_state);
        }
      }
    }
  }
  free(s);
  return new_s;
}
