#include "builder.c"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned long IdxType;

/* if the input string fully matches the pattern */
bool match_full(NFA *nfa, char *input) {
  States *s = new_states();
  push_state(s, 0);
  s = epsilon_closure(nfa, s);

  char *next_char = input;
  while (*next_char != '\0') {
    s = epsilon_closure(nfa, move(nfa, s, *next_char));
    ++next_char;
  }
  bool result = get_shared_states(s, nfa->target_states);
  free(s);
  return result;
}

/*
 * find the first longest match, and copy it to (char *)text, return its length
 */
IdxType match(NFA *nfa, char *input, char *text) {
  States *s = new_states();
  push_state(s, 0);
  s = epsilon_closure(nfa, s);

  IdxType len = 0;
  IdxType last_match = 0;
  char *next_char = input;
  while (*next_char != '\0') {
    s = epsilon_closure(nfa, move(nfa, s, *next_char));

    /*
     * if nothing matches, return to the starting state.
     * however, if there is any match, stop matching and return it as the
     * longest match
     */
    if (states_is_empty(s)) {
      if (last_match > 0)
        break;
      else {
        push_state(s, 0);
        s = epsilon_closure(nfa, s);
      }
    } else {
      text[(len)++] = *next_char;
    }

    /* if any target state is reached, mark matching */
    if (get_shared_states(s, nfa->target_states))
      last_match = len;

    ++next_char;
  }
  len = last_match;
  text[len] = '\0';
  free(s);
  return len;
}

/*
 * similar to `match`, but copy to yytext, assign its length to yyleng,
 * and return the index of the pattern matched
 */
#include "util/yy.c"
int yy_match(NFA *nfa) {
  States *s = new_states();
  push_state(s, 0);
  s = epsilon_closure(nfa, s);

  yyleng = 0;
  IdxType last_match = 0;
  State last_shared_state = 0;
  while (g_buffer_ptr < g_buffer + g_buflen) {
    s = epsilon_closure(nfa, move(nfa, s, *g_buffer_ptr));

    if (states_is_empty(s)) {
      if (last_match > 0)
        break;
      else {
        push_state(s, 0);
        s = epsilon_closure(nfa, s);
      }
    } else {
      yytext[(yyleng)++] = *g_buffer_ptr;
    }

    /* if any target state is reached, mark matching */
    if ((last_shared_state = get_shared_states(nfa->target_states, s))) {
      last_match = yyleng;
    }

    ++g_buffer_ptr;
  }
  yyleng = last_match;
  yytext[yyleng] = '\0';

  for (size_t i = 0; i < nfa->target_states->len; ++i) {
    if (nfa->target_states->states[i] == last_shared_state) {
      free(s);
      return i;
    }
  }

  /* unreachable */
  exit(EXIT_FAILURE);
}
