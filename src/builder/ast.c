#include <stdbool.h>
#include <stdlib.h>

typedef enum AstType {
  LiteralNode,
  SetNode,
  AndNode,
  OrNode,
  RepeatNode,
  SurroundNode,
} AstType;

typedef struct Ast Ast;

typedef struct AstLiteral {
  char value;
} AstLiteral;

typedef struct Vector_char Vector_char;
typedef struct AstSet {
  Vector_char *set;
  bool is_neg;
} AstSet;

typedef struct AstAnd {
  Ast *r1;
  Ast *r2;
} AstAnd;

typedef struct AstOr {
  Ast *r1;
  Ast *r2;
} AstOr;

typedef struct AstRepeat {
  Ast *r;
} AstRepeat;

typedef struct AstSurround {
  Ast *r;
} AstSurround;

typedef union AstData {
  AstLiteral literal;
  AstSet set;
  AstAnd and;
  AstOr or;
  AstRepeat repeat;
  AstSurround surround;
} AstData;

struct Ast {
  AstType type;
  AstData *data;
};

/* newer */
static Ast *new_ast_literal(char value) {
  Ast *node = (Ast *)malloc(sizeof(Ast));
  node->type = LiteralNode;
  node->data = (AstData *)malloc(sizeof(AstData));
  node->data->literal.value = value;
  return node;
}

static Ast *new_ast_set(Vector_char *set, bool is_neg) {
  Ast *node = (Ast *)malloc(sizeof(Ast));
  node->type = SetNode;
  node->data = (AstData *)malloc(sizeof(AstData));
  node->data->set.set = set;
  node->data->set.is_neg = is_neg;
  return node;
}

static Ast *new_ast_and(Ast *r1, Ast *r2) {
  Ast *node = (Ast *)malloc(sizeof(Ast));
  node->type = AndNode;
  node->data = (AstData *)malloc(sizeof(AstData));
  node->data->and.r1 = r1;
  node->data->and.r2 = r2;
  return node;
}

static Ast *new_ast_or(Ast *r1, Ast *r2) {
  Ast *node = (Ast *)malloc(sizeof(Ast));
  node->type = OrNode;
  node->data = (AstData *)malloc(sizeof(AstData));
  node->data->or.r1 = r1;
  node->data->or.r2 = r2;
  return node;
}

static Ast *new_ast_repeat(Ast *r) {
  Ast *node = (Ast *)malloc(sizeof(Ast));
  node->type = RepeatNode;
  node->data = (AstData *)malloc(sizeof(AstData));
  node->data->repeat.r = r;
  return node;
}

static Ast *new_ast_surround(Ast *r) {
  Ast *node = (Ast *)malloc(sizeof(Ast));
  node->type = SurroundNode;
  node->data = (AstData *)malloc(sizeof(AstData));
  node->data->surround.r = r;
  return node;
}

/* clone */
static Ast *clone_ast(Ast *r) {
  if (r == NULL)
    return NULL;
  switch (r->type) {
  case LiteralNode:
    return new_ast_literal(r->data->literal.value);
  case SetNode:
    return new_ast_set(r->data->set.set, r->data->set.is_neg);
  case AndNode:
    return new_ast_and(clone_ast(r->data->and.r1), clone_ast(r->data->and.r2));
  case OrNode:
    return new_ast_or(clone_ast(r->data->or.r1), clone_ast(r->data->or.r2));
  case RepeatNode:
    return new_ast_repeat(clone_ast(r->data->repeat.r));
  case SurroundNode:
    return clone_ast(clone_ast(r->data->surround.r));
  }
  return NULL; /* unreachable */
}

/* free */
void free_ast(Ast *node) {
  if (node == NULL)
    return;
  switch (node->type) {
  case LiteralNode:
    /* nothing to free */
    break;
  case SetNode:
    break;
  case AndNode:
    free_ast(node->data->and.r1);
    free_ast(node->data->and.r2);
    break;
  case OrNode:
    free_ast(node->data->or.r1);
    free_ast(node->data->or.r2);
    break;
  case RepeatNode:
    free_ast(node->data->repeat.r);
    break;
  case SurroundNode:
    free_ast(node->data->surround.r);
    break;
  }
  free(node->data);
  free(node);
}
