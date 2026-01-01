#include <stdio.h>
#include <stdlib.h>

#include "lexer.c"

/*
 * Parser
 */

typedef enum AstType {
  LiteralNode,
  AndNode,
  OrNode,
  RepeatNode,
  SurroundNode,
} AstType;

typedef struct AstLiteral {
  char value;
} AstLiteral;

typedef struct Ast Ast;

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

static Ast *new_ast_any() {
  return new_ast_literal(-2); /* `ANY` defined in nfa.c */
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

typedef struct Parser {
  Lexer *lexer;
  Token *current_token;
} Parser;

Parser *new_parser(Lexer *lexer) {
  Parser *parser = (Parser *)malloc(sizeof(Parser));
  parser->lexer = lexer;
  parser->current_token = get_next_token(lexer);
  return parser;
}

static void eat(Parser *parser, TokenType type) {
  if (parser->current_token->type == type) {
    parser->current_token = get_next_token(parser->lexer);
  } else {
    printf("Wrong Token Type! Expect %d, found %d.\n", type,
           parser->current_token->type);
    exit(1);
  }
}

/* Forward declarations */
static Ast *parse_expr(Parser *parser);
static Ast *parse_term(Parser *parser);
static Ast *parse_factor(Parser *parser);
static Ast *parse_base(Parser *parser);

/*
 * expr := term*
 */
static Ast *parse_expr(Parser *parser) {
  Ast *node = parse_term(parser);
  while (parser->current_token->type == LITERAL ||
         parser->current_token->type == DOT ||
         parser->current_token->type == LPAREN) {
    Ast *right = parse_term(parser);
    node = new_ast_and(node, right);
  }
  return node;
}

/*
 * term := factor ('|' factor)*
 */
static Ast *parse_term(Parser *parser) {
  Ast *node = parse_factor(parser);
  while (parser->current_token->type == OR) {
    eat(parser, OR);
    Ast *right = parse_factor(parser);
    node = new_ast_or(node, right);
  }
  return node;
}

/*
 * factor := base ('*')
 *           base ('+')
 */
static Ast *parse_factor(Parser *parser) {
  Ast *node = parse_base(parser);
  if (parser->current_token->type == REPEAT) {
    eat(parser, REPEAT);
    node = new_ast_repeat(node);
  } else if (parser->current_token->type == ONE_OR_MORE) {
    eat(parser, ONE_OR_MORE);
    node = new_ast_and(clone_ast(node), new_ast_repeat(node));
  }
  return node;
}

/*
 * base := LITERAL
 *       | DOT
 *       | '(' expr ')'
 */
static Ast *parse_base(Parser *parser) {
  if (parser->current_token->type == LITERAL) {
    char value = parser->current_token->value;
    eat(parser, LITERAL);
    return new_ast_literal(value);
  } else if (parser->current_token->type == DOT) {
    eat(parser, DOT);
    return new_ast_any();
  } else if (parser->current_token->type == LPAREN) {
    eat(parser, LPAREN);
    Ast *node = parse_expr(parser);
    eat(parser, RPAREN);
    return new_ast_surround(node);
  } else {
    printf("unexpected token: %d\n", parser->current_token->type);
    exit(1);
  }
}

/* Entry point for parsing */
Ast *parse(Parser *parser) {
  Ast *node = parse_expr(parser);
  if (parser->current_token->type != END) {
    printf("unexpected trailing token: %d\n", parser->current_token->type);
    exit(1);
  }
  return node;
}
