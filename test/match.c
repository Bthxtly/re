#include "../src/match.c"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

void match_one_pattern() {
  NFA *nfa = build("fo(o|ba*r)*baz");

  assert(match_string(nfa, "fobaz"));
  assert(match_string(nfa, "foorbaz"));
  assert(match_string(nfa, "fobrbaz"));
  assert(match_string(nfa, "fooaaaarbaz"));
  assert(match_string(nfa, "foorororbrbrbaz"));
  assert(match_string(nfa, "fooaaaarbarbrbaz"));

  free(nfa);
}

void match_multiple_patterns() {
  char *patterns[] = {"foo", "foooo", "fo*b"};
  size_t len = sizeof(patterns) / sizeof(char *);
  NFA *nfa = build_many(patterns, len);

  assert(match_string(nfa, "foo"));
  assert(match_string(nfa, "foooo"));
  assert(match_string(nfa, "fb"));
  assert(match_string(nfa, "foooooob"));
  assert(!match_string(nfa, "fo"));
  assert(!match_string(nfa, "fooo"));
  assert(!match_string(nfa, "fbi"));
}

int main(int argc, char *argv[]) {
  match_one_pattern();
  match_multiple_patterns();

  printf("All tests in match.c pass!\n");
  return EXIT_SUCCESS;
}
