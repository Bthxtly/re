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
  size_t len = sizeof(patterns) / sizeof(char *);
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
  char *patterns[] = {
      "Lorem",      "ipsum", "dolor", "sit", "amet",    "consectetur",
      "adipiscing", "elit",  "sed",   "do",  "eiusmod", "tempor",
  };
  size_t len = sizeof(patterns) / sizeof(char *);
  NFA *nfa = build_many(patterns, len);

  assert(match(nfa, "foosed"));
  assert(match(nfa, "fLoremmm"));
  assert(match(nfa, "dolor"));
  assert(!match(nfa, "ame"));
  assert(!match(nfa, "empo"));
}

void yy() {
  char *patterns[] = {"foo", "foooo", "fo*b"};
  size_t len = sizeof(patterns) / sizeof(char *);
  NFA *nfa = build_many(patterns, len);

  char yytext[10];
  size_t yyleng;

  assert(yy_match(nfa, "bfooooob", yytext, &yyleng));
  assert(strcmp(yytext, "fooooob") == 0);
  assert(yyleng == 7);

  assert(yy_match(nfa, "bfoooa", yytext, &yyleng));
  assert(strcmp(yytext, "foo") == 0);
  assert(yyleng == 3);

  assert(yy_match(nfa, "bfoooooa", yytext, &yyleng));
  assert(strcmp(yytext, "foooo") == 0);
  assert(yyleng == 5);
}

int main(int argc, char *argv[]) {
  match_one_pattern();
  match_multiple_patterns();
  match_partitially();
  yy();

  printf("All tests in match.c pass!\n");
  return EXIT_SUCCESS;
}
