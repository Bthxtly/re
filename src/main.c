#include "match.c"
#include <stdio.h>
#include <string.h>

int main() {
  printf("Input a Regular Expression:\n");
  char buffer[256];
  if (scanf("%s", buffer) != 1)
    return 0;

  NFA *nfa = build(buffer);

  for (;;) {
    printf("Input string to be matched with RE(press q to quit):\n");
    if (scanf("%s", buffer) != 1 || strcmp(buffer, "q") == 0)
      break;

    if (match_string(nfa, buffer)) {
      printf("Matches!\n");
    } else {
      printf("Doesn't match.\n");
    }
  }
  free_nfa(nfa);
  return 0;
}
