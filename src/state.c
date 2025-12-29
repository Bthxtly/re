#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX 99
#define bool char
#define true 1
#define false 0

typedef unsigned short State;

typedef struct States {
  State states[MAX];
  size_t len;
} States;

/* create an empty container of states */
States *new_states() {
  States *s = (States *)malloc(sizeof(States));
  s->len = 0;
  return s;
}

/* push a state into the container */
void push_state(States *s, State state) {
  s->states[s->len] = state;
  ++(s->len);
}

/* if the container has the state */
bool have_state(States *s, State state) {
  for (size_t i = 0; i < s->len; ++i)
    if (s->states[i] == state)
      return true;
  return false;
}

/* if two containers have the same state */
bool have_same_state(States *s1, States *s2) {
  for (size_t i = 0; i < s1->len; ++i) {
    for (size_t j = 0; j < s2->len; ++j) {
      if (s1->states[i] == s2->states[j])
        return true;
    }
  }
  return false;
}

/* print states of the container */
void print_states(States *s) {
  printf("States[");
  if (s->len == 0) {
    printf("]\n");
  } else if (s->len == 1) {
    printf("%hu]\n", s->states[0]);
  } else {
    for (size_t i = 0; i < s->len - 1; ++i) {
      printf("%hu, ", s->states[i]);
    }
    printf("%hu]\n", s->states[s->len - 1]);
  }
}
