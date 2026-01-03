#include "state.c"
#include <stdbool.h>

#define TYPE char
#include "util/vector.c"

typedef struct Label {
  enum {
    CHAR,
    SET,
    NEG_SET,
  } type;

  union {
    char symbol;
    Vector_char *set;
  } data;
} Label;

Label *new_literal_label(char symbol) {
  Label *label = (Label *)malloc(sizeof(Label));
  label->type = CHAR;
  label->data.symbol = symbol;
  return label;
}

Label *new_set_label(Vector_char *set, bool is_neg) {
  Label *label = (Label *)malloc(sizeof(Label));
  label->type = (is_neg ? NEG_SET : SET);
  label->data.set = set;
  return label;
}

typedef struct Edge {
  Label *label;
  State from;
  State to;
} Edge;

/* create a new edge */
Edge *new_edge(Label *label, State from, State to) {
  Edge *e = (Edge *)malloc(sizeof(Edge));
  e->label = label;
  e->from = from;
  e->to = to;
  return e;
}
