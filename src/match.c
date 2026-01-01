#include "builder.c"
#include <stddef.h>

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
  bool result = have_same_state(s, nfa->target_states);
  free(s);
  return result;
}

bool match(NFA *nfa, char *input) {
  States *s = new_states();
  push_state(s, 0);
  s = epsilon_closure(nfa, s);

  bool is_matched = false;
  char *next_char = input;
  while (*next_char != '\0') {
    s = epsilon_closure(nfa, move(nfa, s, *next_char));

    /*
     * if nothing matches, return to the starting state.
     * however, if there is any match, stop matching and return it as the
     * longest match
     */
    if (states_is_empty(s)) {
      if (is_matched)
        return true;
      else {
        push_state(s, 0);
        s = epsilon_closure(nfa, s);
      }
    }

    /* if any target state is reached, mark matching */
    if (have_same_state(s, nfa->target_states))
      is_matched = true;

    ++next_char;
  }
  free(s);
  return is_matched;
}

/*
 * find the first longest match, and copy it to yytext, assign the length of
 * match to yyleng
 */
bool yy_match(NFA *nfa, char *input, char *yytext, size_t *yyleng) {
  States *s = new_states();
  push_state(s, 0);
  s = epsilon_closure(nfa, s);

  *yyleng = 0;
  size_t last_match = 0;
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
      yytext[(*yyleng)++] = *next_char;
    }

    /* if any target state is reached, mark matching */
    if (have_same_state(s, nfa->target_states))
      last_match = *yyleng;

    ++next_char;
  }
  *yyleng = last_match;
  yytext[*yyleng] = '\0';
  free(s);
  return last_match > 0;
}
