#include <stdio.h>
#include <stdlib.h>

#include "ast.c"
#include "lexer.c"

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
