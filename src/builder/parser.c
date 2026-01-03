#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "ast.c"
#include "lexer.c"

/* pre-define */
Vector_char *new_vector_char();
int push_vector_char(Vector_char *vec, char value);

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
static Ast *parse_range(Parser *parser);

/*
 * expr := term*
 */
static Ast *parse_expr(Parser *parser) {
  Ast *node = parse_term(parser);
  while (parser->current_token->type == LITERAL ||
         parser->current_token->type == CARET ||
         parser->current_token->type == DOT ||
         parser->current_token->type == LBRACKET ||
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
 * base := LITERAL | CARET
 *       | DOT
 *       | '[' range ']'
 *       | '(' expr ')'
 */
static Ast *parse_base(Parser *parser) {
  switch (parser->current_token->type) {
  case LITERAL: {
    char value = parser->current_token->value;
    eat(parser, LITERAL);
    return new_ast_literal(value);
  }
  case CARET: {
    char value = parser->current_token->value;
    eat(parser, CARET);
    return new_ast_literal(value);
  }
  case DOT: {
    /* anything but newline */
    eat(parser, DOT);
    Vector_char *set = new_vector_char();
    push_vector_char(set, '\n');
    return new_ast_set(set, true);
  }
  case LBRACKET: {
    eat(parser, LBRACKET);
    Ast *node = parse_range(parser);
    eat(parser, RBRACKET);
    return node;
  }
  case LPAREN: {
    eat(parser, LPAREN);
    Ast *node = parse_expr(parser);
    eat(parser, RPAREN);
    return new_ast_surround(node);
  }
  default: {
    printf("unexpected token: %d\n", parser->current_token->type);
    exit(1);
  }
  }
}

/*
 * range or set
 * range := CARET
 *        | LITERAL
 *        | LITERAL DASH LITERAL
 *        | range
 */
static Ast *parse_range(Parser *parser) {
  /* parse negate ^ */
  bool is_neg = false;
  if (parser->current_token->type == CARET) {
    is_neg = true;
    eat(parser, CARET);
  }

  Vector_char *set = new_vector_char();
  while (parser->current_token->type != RBRACKET) {
    char from = parser->current_token->value;
    eat(parser, LITERAL);
    Ast *right;
    /* range */
    if (parser->current_token->type == DASH) {
      eat(parser, DASH);
      char to = parser->current_token->value;
      eat(parser, LITERAL);
      for (char c = from; c <= to; ++c)
        push_vector_char(set, c);
    }
    /* set */
    else
      push_vector_char(set, from);
  }
  return new_ast_set(set, is_neg);
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
