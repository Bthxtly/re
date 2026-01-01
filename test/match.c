#include "../src/match.c"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void match_one_pattern() {
  NFA *nfa = build("fo(o|ba*r)*baz");

  assert(match_full(nfa, "fobaz"));
  assert(match_full(nfa, "foorbaz"));
  assert(match_full(nfa, "fobrbaz"));
  assert(match_full(nfa, "fooaaaarbaz"));
  assert(match_full(nfa, "foorororbrbrbaz"));
  assert(match_full(nfa, "fooaaaarbarbrbaz"));

  free(nfa);
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
}

int main(int argc, char *argv[]) {
  match_one_pattern();
  match_multiple_patterns();
  match_partitially();
  yy();

  printf("All tests in match.c pass!\n");
  return EXIT_SUCCESS;
}
