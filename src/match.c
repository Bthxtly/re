#include "builder.c"

/* if the NFA matches the given input string */
bool match_string(NFA *nfa, char *input) {
  States *s = new_states();
  push_state(s, 0);
  s = epsilon_closure(nfa, s);

  char *next_char = input;
  while (*next_char != '\0') {
    s = epsilon_closure(nfa, move(nfa, s, *next_char));
    ++next_char;
  }
  bool result = have_same_state(s, nfa->target_states);
  free(s);
  return result;
}
