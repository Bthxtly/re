#include <stdlib.h>

/*
 * Lexer
 */

typedef enum TokenType {
  LPAREN,
  RPAREN,
  LBRACKET,
  RBRACKET,
  CARET,
  DASH,
  DOT,
  PLUS,
  ASTERISK,
  BAR,
  BACK_SLASH,
  LITERAL,
  END
} TokenType;

typedef struct Token {
  TokenType type;
  char value; /* valid only for LITERAL type */
} Token;

/* create a token */
Token *new_token(TokenType type, char value) {
  Token *token = (Token *)malloc(sizeof(Token));
  token->type = type;
  token->value = value;
  return token;
}

typedef struct Lexer {
  char *pattern;
  char *current_char;
  Token *current_token;
} Lexer;

/* create a new lexer from pattern string */
Lexer *new_lexer(char *pattern) {
  Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
  lexer->pattern = pattern;
  lexer->current_char = lexer->pattern;
  lexer->current_token = NULL;
  return lexer;
}

Token *get_next_token(Lexer *lexer) {
  if (lexer->current_token != NULL)
    free(lexer->current_token);

  char current_char = *lexer->current_char;
  switch (current_char) {
  case '\0':
    lexer->current_token = new_token(END, '\0');
    break;
  case '(':
    lexer->current_token = new_token(LPAREN, '(');
    break;
  case ')':
    lexer->current_token = new_token(RPAREN, ')');
    break;
  case '[':
    lexer->current_token = new_token(LBRACKET, '[');
    break;
  case ']':
    lexer->current_token = new_token(RBRACKET, ']');
    break;
  case '^':
    lexer->current_token = new_token(CARET, '^');
    break;
  case '-':
    lexer->current_token = new_token(DASH, '-');
    break;
  case '.':
    lexer->current_token = new_token(DOT, '.');
    break;
  case '+':
    lexer->current_token = new_token(PLUS, '+');
    break;
  case '*':
    lexer->current_token = new_token(ASTERISK, '*');
    break;
  case '|':
    lexer->current_token = new_token(BAR, '|');
    break;
  case '\\':
    lexer->current_token = new_token(BACK_SLASH, '\\');
    break;
  default:
    lexer->current_token = new_token(LITERAL, current_char);
    break;
  }
  ++(lexer->current_char);

  return lexer->current_token;
}
