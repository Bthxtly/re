#include "builder/parser.c"
#include "nfa.c"
#include <stddef.h>
#include <stdlib.h>

static State g_state_counts = 0;

typedef struct {
  NFA *nfa;
  State start;
  State accept;
} NFAFragment;

/* create a new NFA fragment */
static NFAFragment *new_nfa_fragment(NFA *nfa, State start, State accept) {
  NFAFragment *fragment = (NFAFragment *)malloc(sizeof(NFAFragment));
  fragment->nfa = nfa;
  fragment->start = start;
  fragment->accept = accept;
  return fragment;
}

/* get current state counts */
static State get_state_counts() { return g_state_counts; }

/* increase states counts and get the latest state number */
static State increase_state_counts() { return g_state_counts++; }

/* decrease states counts by one, used to concatenate two NFA */
static void decrease_state_counts() { --g_state_counts; }

/* add an ε-labled edge to the NFA */
static void add_epsilon(NFA *nfa, State from, State to) {
  push_edge(nfa, new_edge(EPSILON, from, to));
}

/* add a symbol-labled edge to the NFA */
static void add_symbol(NFA *nfa, State from, State to, char symbol) {
  push_edge(nfa, new_edge(symbol, from, to));
}

/* move all edges from source NFA to destination NFA */
static void move_edges(NFA *dst, NFA *src) {
  for (size_t i = 0; i < src->edges_count; ++i) {
    push_edge(dst, src->edges[i]);
  }
  src->edges_count = 0;
}

static NFAFragment *ast2nfa_fragment(Ast *ast) {
  if (ast == NULL)
    return NULL;

  switch (ast->type) {
  case LiteralNode: {
    /* START --literal--> END */
    NFA *nfa = new_nfa();
    State start = increase_state_counts();
    State accept = increase_state_counts();
    add_symbol(nfa, start, accept, ast->data->literal.value);
    return new_nfa_fragment(nfa, start, accept);
  }

  case AndNode: {
    /* START --left--> (left end & right start) --right--> END */
    NFAFragment *left = ast2nfa_fragment(ast->data->and.r1);
    decrease_state_counts(); /* concatenate left end and right start */
    NFAFragment *right = ast2nfa_fragment(ast->data->and.r2);
    move_edges(left->nfa, right->nfa);
    free(right->nfa);
    NFAFragment *result =
        new_nfa_fragment(left->nfa, left->start, right->accept);
    free(left);
    free(right);
    return result;
  }

  case OrNode: {
    /*
     *          /-ε--> S₀ --left---> S₁ -ε-\
     * START --<                            >--> END
     *          \-ε--> S₂ --right--> S₃ -ε-/
     */
    NFA *nfa = new_nfa();
    State start = increase_state_counts();
    NFAFragment *left = ast2nfa_fragment(ast->data->or.r1);
    NFAFragment *right = ast2nfa_fragment(ast->data->or.r2);
    State accept = increase_state_counts();
    add_epsilon(nfa, start, left->start);
    add_epsilon(nfa, start, right->start);
    add_epsilon(nfa, left->accept, accept);
    add_epsilon(nfa, right->accept, accept);
    move_edges(nfa, left->nfa);
    move_edges(nfa, right->nfa);
    free(left->nfa);
    free(right->nfa);
    NFAFragment *result = new_nfa_fragment(nfa, start, accept);
    free(left);
    free(right);
    return result;
  }

  case RepeatNode: {
    /*
     *               .-<-ε-<-.
     *              /         \
     * START --ε--> S₀ --r--> S₁ --ε--> END
     *     \                            /
     *      .---------->-ε->-----------.
     */
    NFA *nfa = new_nfa();
    State start = increase_state_counts();
    NFAFragment *body = ast2nfa_fragment(ast->data->repeat.r);
    State accept = increase_state_counts();
    move_edges(nfa, body->nfa);
    add_epsilon(nfa, start, body->start);
    add_epsilon(nfa, start, accept);
    add_epsilon(nfa, body->accept, body->start);
    add_epsilon(nfa, body->accept, accept);
    free(body->nfa);
    NFAFragment *result = new_nfa_fragment(nfa, start, accept);
    free(body);
    return result;
  }

  case SurroundNode: {
    /* START --r--> END */
    return ast2nfa_fragment(ast->data->surround.r);
  }

  default:
    exit(1);
  }
}

NFA *ast2nfa(Ast *ast) {
  g_state_counts = 0;
  NFAFragment *fragment = ast2nfa_fragment(ast);
  NFA *nfa = fragment->nfa;
  free(fragment);
  free_ast(ast);

  nfa->states_count = g_state_counts;

  States *target_states = new_states();
  push_state(target_states, g_state_counts - 1);
  nfa->target_states = target_states;
  return nfa;
}

NFA *build(char *input) {
  Lexer *lexer = new_lexer(input);
  Parser *parser = new_parser(lexer);
  Ast *ast = parse(parser);
  NFA *nfa = ast2nfa(ast);
  if (lexer->current_token != NULL) {
    free(lexer->current_token);
  }
  free(lexer);
  free(parser);
  return nfa;
}
