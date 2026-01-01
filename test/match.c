#include "../src/match.c"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void match_one_pattern() {
  g_state_counts = 0;
  NFA *nfa = build("fo(o|ba*r)*baz");

  assert(match_full(nfa, "fobaz"));
  assert(match_full(nfa, "foorbaz"));
  assert(match_full(nfa, "fobrbaz"));
  assert(match_full(nfa, "fooaaaarbaz"));
  assert(match_full(nfa, "foorororbrbrbaz"));
  assert(match_full(nfa, "fooaaaarbarbrbaz"));

  free_nfa(nfa);
}

void match_multiple_patterns() {
  char *patterns[] = {"foo", "foooo", "fo*b"};
  IdxType len = sizeof(patterns) / sizeof(char *);
  NFA *nfa = build_many(patterns, len);

  assert(match_full(nfa, "foo"));
  assert(match_full(nfa, "foooo"));
  assert(match_full(nfa, "fb"));
  assert(match_full(nfa, "foooooob"));
  assert(!match_full(nfa, "fo"));
  assert(!match_full(nfa, "fooo"));
  assert(!match_full(nfa, "fbi"));

  free_nfa(nfa);
}

void match_partitially() {
  char *patterns[] = {"foo", "foooo", "fo*b"};
  IdxType len = sizeof(patterns) / sizeof(char *);
  NFA *nfa = build_many(patterns, len);

  char text[10];

  assert(match(nfa, "bfooooob", text) == 7);
  assert(strcmp(text, "fooooob") == 0);

  assert(match(nfa, "bfoooa", text) == 3);
  assert(strcmp(text, "foo") == 0);

  assert(match(nfa, "bfoooooa", text) == 5);
  assert(strcmp(text, "foooo") == 0);

  free_nfa(nfa);
}

void yy() {
  char *patterns[] = {"foo", "foooo", "fo*b"};
  IdxType len = sizeof(patterns) / sizeof(char *);
  NFA *nfa = build_many(patterns, len);

  g_buffer = "fooobaz";
  g_buflen = 7;
  g_buffer_ptr = g_buffer;

  assert(yy_match(nfa) == 2);
  assert(*g_buffer_ptr == 'a');

  free_nfa(nfa);
}

bool build_and_match(char *pattern, char *input) {
  g_state_counts = 0;
  NFA *nfa = build(pattern);
  bool result = match_full(nfa, input);
  free_nfa(nfa);
  return result;
}

void extended_rules() {
  assert(build_and_match("f.o", "foo"));
  assert(build_and_match("f.+o", "farstdhneio"));
  assert(!build_and_match("f.+o", "farstdhneiob"));
  assert(build_and_match("f(.o|(ar)+)|z", "fz"));
  assert(build_and_match("f(.o|(ar)+)|z", "fxo"));
  assert(build_and_match("f(.o|(ar)+)|z", "fxar"));
  assert(build_and_match("f(.o|(ar)*)|z", "fx"));
  assert(!build_and_match("f(.o|(ar)+)|z", "fx"));
}

int main(int argc, char *argv[]) {
  match_one_pattern();
  match_multiple_patterns();
  match_partitially();
  yy();
  extended_rules();

  printf("All tests in match.c pass!\n");
  return EXIT_SUCCESS;
}
