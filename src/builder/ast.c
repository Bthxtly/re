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
typedef struct Vector_char Vector_char;

typedef struct Ast {
  enum { AstLiteral, AstSet, AstAnd, AstOr, AstRepeat, AstSurround } type;

  union {
    struct AstLiteral {
      char value;
    } AstLiteral;

    struct AstSet {
      Vector_char *set;
      bool is_neg;
    } AstSet;

    struct AstAnd {
      Ast *r1;
      Ast *r2;
    } AstAnd;

    struct AstOr {
      Ast *r1;
      Ast *r2;
    } AstOr;

    struct AstRepeat {
      Ast *r;
    } AstRepeat;

    struct AstSurround {
      Ast *r;
    } AstSurround;
  } data;
} Ast;

Ast *new_ast(Ast ast) {
  Ast *p = malloc(sizeof(Ast));
  if (p != NULL)
    *p = ast;
  return p;
}

#define NEW_AST(tag, ...)                                                      \
  new_ast((Ast){tag, {.tag = (struct tag){__VA_ARGS__}}})

/* newer */
static Ast *new_ast_literal(char value) { return NEW_AST(AstLiteral, value); }

static Ast *new_ast_set(Vector_char *set, bool is_neg) {
  return NEW_AST(AstSet, set, is_neg);
}

static Ast *new_ast_and(Ast *r1, Ast *r2) { return NEW_AST(AstAnd, r1, r2); }

static Ast *new_ast_or(Ast *r1, Ast *r2) { return NEW_AST(AstOr, r1, r2); }

static Ast *new_ast_repeat(Ast *r) { return NEW_AST(AstRepeat, r); }

static Ast *new_ast_surround(Ast *r) { return NEW_AST(AstSurround, r); }

/* clone */
static Ast *clone_ast(Ast *r) {
  if (r == NULL)
    return NULL;
  switch (r->type) {
  case LiteralNode:
    return new_ast_literal(r->data.AstLiteral.value);
  case SetNode:
    return new_ast_set(r->data.AstSet.set, r->data.AstSet.is_neg);
  case AndNode:
    return new_ast_and(clone_ast(r->data.AstAnd.r1),
                       clone_ast(r->data.AstAnd.r2));
  case OrNode:
    return new_ast_or(clone_ast(r->data.AstOr.r1), clone_ast(r->data.AstOr.r2));
  case RepeatNode:
    return new_ast_repeat(clone_ast(r->data.AstRepeat.r));
  case SurroundNode:
    return clone_ast(clone_ast(r->data.AstSurround.r));
  }
  return NULL; /* unreachable */
}

/* comparison */
bool equal_ast(Ast *a, Ast *b) {
  if (a == b)
    return true;
  if (a == NULL || b == NULL)
    return false;
  if (a->type != b->type)
    return false;

  switch (a->type) {
  case LiteralNode:
    return a->data.AstLiteral.value == b->data.AstLiteral.value;
  case SetNode:
    return true; /* ignore it */
  case AndNode:
    return equal_ast(a->data.AstAnd.r1, b->data.AstAnd.r1) &&
           equal_ast(a->data.AstAnd.r2, b->data.AstAnd.r2);
  case OrNode:
    return equal_ast(a->data.AstOr.r1, b->data.AstOr.r1) &&
           equal_ast(a->data.AstOr.r2, b->data.AstOr.r2);
  case RepeatNode:
    return equal_ast(a->data.AstRepeat.r, b->data.AstRepeat.r);
  case SurroundNode:
    return equal_ast(a->data.AstSurround.r, b->data.AstSurround.r);
  default:
    return false;
  }
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
    /* nothing to free */
    break;
  case AndNode:
    free_ast(node->data.AstAnd.r1);
    free_ast(node->data.AstAnd.r2);
    break;
  case OrNode:
    free_ast(node->data.AstOr.r1);
    free_ast(node->data.AstOr.r2);
    break;
  case RepeatNode:
    free_ast(node->data.AstRepeat.r);
    break;
  case SurroundNode:
    free_ast(node->data.AstSurround.r);
    break;
  }
  free(node);
}
