#include "../src/builder.c"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>

void tokenize() {
  char *input = "fo(o|ba*r)*baz";
  Lexer *lexer = new_lexer(input);
  TokenType types[] = {
      LITERAL, /* f */
      LITERAL, /* o */
      LPAREN,  /* ( */
      LITERAL, /* o */
      OR,      /* | */
      LITERAL, /* b */
      LITERAL, /* a */
      REPEAT,  /* * */
      LITERAL, /* r */
      RPAREN,  /* ) */
      REPEAT,  /* * */
      LITERAL, /* b */
      LITERAL, /* a */
      LITERAL, /* z */
  };
  size_t i = 0;
  Token *token;
  while ((token = get_next_token(lexer))->type != END) {
    assert(token->type == types[i]);
    ++i;
  }
  free(token);
  free(lexer);
}

void test_ast() {
  char *input = "fo(o|ba*r)*baz";
  Lexer *lexer = new_lexer(input);
  Parser *parser = new_parser(lexer);
  Ast *ast = parse(parser);

  /*
   *  And
   *  ├ And
   *  │ ├ And
   *  │ │ ├ And
   *  │ │ │ ├ And
   *  │ │ │ │ ├ Literal('f')
   *  │ │ │ │ └ Literal('o')
   *  │ │ │ └ Repeat
   *  │ │ │   └ Surround
   *  │ │ │     └ And
   *  │ │ │       ├ And
   *  │ │ │       │ ├ Or
   *  │ │ │       │ │ ├ Literal('o')
   *  │ │ │       │ │ └ Literal('b')
   *  │ │ │       │ └ Repeat
   *  │ │ │       │   └ Literal('a')
   *  │ │ │       └ And
   *  │ │ │         └ Literal('r')
   *  │ │ └ Literal('b')
   *  │ └ Literal('a')
   *  └ Literal('z')
   */

  assert(ast->type == AndNode);
  assert(ast->data->and.r1->type == AndNode);
  assert(ast->data->and.r1->data->and.r1->type == AndNode);
  Ast *fourth_ast = ast->data->and.r1->data->and.r1->data->and.r1;
  assert(fourth_ast->type == AndNode);

  Ast *fifth_ast = fourth_ast->data->and.r1;
  assert(fifth_ast->type == AndNode);
  assert(fifth_ast->data->and.r1->type == LiteralNode);
  assert(fifth_ast->data->and.r1->data->literal.value == 'f');
  assert(fifth_ast->data->and.r2->data->literal.value == 'o');

  fifth_ast = fourth_ast->data->and.r2;
  assert(fifth_ast->type == RepeatNode);
  assert(fifth_ast->data->repeat.r->type == SurroundNode);

  Ast *seventh_ast = fifth_ast->data->repeat.r->data->surround.r;
  assert(seventh_ast->type == AndNode);
  assert(seventh_ast->data->and.r1->type == AndNode);

  Ast *ninth_ast = seventh_ast->data->and.r1->data->and.r1;
  assert(ninth_ast->type == OrNode);
  assert(ninth_ast->data->or.r1->type == LiteralNode);
  assert(ninth_ast->data->or.r1->data->literal.value == 'o');
  assert(ninth_ast->data->or.r2->type == LiteralNode);
  assert(ninth_ast->data->or.r2->data->literal.value == 'b');

  assert(seventh_ast->data->and.r2->type == LiteralNode);
  assert(seventh_ast->data->and.r2->data->literal.value == 'r');

  fourth_ast = ast->data->and.r1->data->and.r1->data->and.r2;
  assert(fourth_ast->type == LiteralNode);
  assert(fourth_ast->data->literal.value == 'b');

  assert(ast->data->and.r1->data->and.r2->type == LiteralNode);
  assert(ast->data->and.r1->data->and.r2->data->literal.value == 'a');

  assert(ast->data->and.r2->type == LiteralNode);
  assert(ast->data->and.r2->data->literal.value == 'z');

  free_ast(ast);
  if (lexer->current_token != NULL) {
    free(lexer->current_token);
  }
  free(lexer);
  free(parser);
}

void test_nfa() {
  NFA *nfa = build("fo(o|ba*r)*baz");
  // print_edges(nfa);
  // clang-format off
  /*
   *                                      /-<---------------------<-ε-<---------------------------<-\
   *                                     /                                                           \
   *                                    | /-ε--> 4 --o--> 5 -ε-\         /-<-ε-<-\                    |
   * START--> 0 --f--> 1 --o--> 2 --ε--> 3                      8 --ε--> 9 --a--> 10 --ε-> 11 --r--> 12 --ε--> 13 --b--> 14 --a--> 15 --z--> 16 --> END
   *                            |         \-ε--> 6 --b--> 7 -ε-/ \--------->-ε->----------/                     |
   *                            \                                                                              /
   *                             \->------------------------------>-ε->------------------------------------->-/
   *
   */
  // clang-format on
  assert(nfa->states_count == 17);
  free_nfa(nfa);
}

int main() {
  tokenize();
  test_ast();
  test_nfa();

  printf("All tests in builder.c pass!\n");
  return EXIT_SUCCESS;
}
