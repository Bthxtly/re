#include "../src/builder.c"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>

void tokenize() {
  char *pattern = "fo(o|ba*r)*baz";
  Lexer *lexer = new_lexer(pattern);
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
  char *pattern = "fo(o|ba*r)*baz";
  Lexer *lexer = new_lexer(pattern);
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
   *  │ │ │       └ Literal('r')
   *  │ │ └ Literal('b')
   *  │ └ Literal('a')
   *  └ Literal('z')
   */

  // clang-format off
  assert(equal_ast(
      ast,
      new_ast_and(
          new_ast_and(
              new_ast_and(
                  new_ast_and(
                      new_ast_and(
                          new_ast_literal('f'),
                          new_ast_literal('o')),
                      new_ast_repeat(
                          new_ast_surround(
                              new_ast_and(
                                  new_ast_and(
                                      new_ast_or(
                                          new_ast_literal('o'),
                                          new_ast_literal('b')),
                                      new_ast_repeat(
                                          new_ast_literal('a'))),
                          new_ast_literal('r'))))),
              new_ast_literal('b')),
          new_ast_literal('a')),
      new_ast_literal('z'))));
  // clang-format on

  free_ast(ast);
  if (lexer->current_token != NULL) {
    free(lexer->current_token);
  }
  free(lexer);
  free(parser);
}

void test_ast_set() {
  char *pattern = "[^0-9]";
  Lexer *lexer = new_lexer(pattern);
  Parser *parser = new_parser(lexer);
  Ast *ast = parse(parser);

  assert(ast->type == SetNode);
  assert(ast->data.AstSet.is_neg == true);
  assert(ast->data.AstSet.set->size == 10);
  assert(ast->data.AstSet.set->data[0] == '0');
  assert(ast->data.AstSet.set->data[1] == '1');
  assert(ast->data.AstSet.set->data[2] == '2');
  assert(ast->data.AstSet.set->data[3] == '3');
  assert(ast->data.AstSet.set->data[4] == '4');
  assert(ast->data.AstSet.set->data[5] == '5');
  assert(ast->data.AstSet.set->data[6] == '6');
  assert(ast->data.AstSet.set->data[7] == '7');
  assert(ast->data.AstSet.set->data[8] == '8');
  assert(ast->data.AstSet.set->data[9] == '9');
}

void test_nfa() {
  NFA *nfa = build("fo(o|ba*r)*baz");
  /* print_edges(nfa); */
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
  test_ast_set();
  test_nfa();

  printf("All tests in builder.c pass!\n");
  return EXIT_SUCCESS;
}
