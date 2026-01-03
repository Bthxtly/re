#include <assert.h>
#include <stdio.h>

#include "../src/match.c"

/* NFA derived from regular expression `(a|b)*` */
NFA *init_nfa() {
  char EPS = EPSILON;
  NFA *nfa = new_nfa();
  set_states_count(nfa, 8);
  States *target_states = new_states();
  push_state(target_states, 7);
  nfa->target_states = target_states;
  push_edge(nfa, new_edge(new_literal_label(EPS), 1, 2));
  push_edge(nfa, new_edge(new_literal_label(EPS), 1, 4));
  push_edge(nfa, new_edge(new_literal_label(EPS), 3, 6));
  push_edge(nfa, new_edge(new_literal_label(EPS), 5, 6));
  push_edge(nfa, new_edge(new_literal_label('a'), 2, 3));
  push_edge(nfa, new_edge(new_literal_label('b'), 4, 5));
  push_edge(nfa, new_edge(new_literal_label(EPS), 0, 1));
  push_edge(nfa, new_edge(new_literal_label(EPS), 0, 7));
  push_edge(nfa, new_edge(new_literal_label(EPS), 6, 1));
  push_edge(nfa, new_edge(new_literal_label(EPS), 6, 7));

  assert(nfa->states_count == 8);
  assert(nfa->edges_count == 10);
  assert(nfa->target_states->len == 1);
  assert(nfa->target_states->states[0] == 7);

  return nfa;
}

/* test ε-closure(T) and move(T, a) */
void basic_operations(NFA *nfa) {
  States *s = new_states();
  push_state(s, 0);
  assert(s->len == 1);
  assert(s->states[0] == 0);

  s = epsilon_closure(nfa, s);
  assert(s->len == 5); /* 0, 1, 2, 4, 7 */

  s = move(nfa, s, 'a');
  assert(s->len == 1); /* 3 */
  assert(s->states[0] == 3);

  s = epsilon_closure(nfa, s);
  assert(s->len == 6); /* 1, 2, 3, 4, 6, 7 */
}

int main(int argc, char *argv[]) {
  NFA *nfa = init_nfa();

  basic_operations(nfa);

  assert(match_full(nfa, "aabb"));
  assert(!match_full(nfa, "abc"));

  free(nfa);

  printf("All tests in nfa.c pass!\n");
  return EXIT_SUCCESS;
}
