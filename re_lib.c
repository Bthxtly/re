/*
 * Merged regex library
 * For embedding into lers projects
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * ============================================================================
 * state.c - State management
 * ============================================================================
 */

#define MAX 9999
#define bool char
#define true 1
#define false 0

typedef unsigned short State;

typedef struct States {
  State states[MAX];
  size_t len;
} States;

/* create an empty container of states */
States *new_states() {
  States *s = (States *)malloc(sizeof(States));
  s->len = 0;
  return s;
}

/* push a state into the container */
void push_state(States *s, State state) {
  s->states[s->len] = state;
  ++(s->len);
}

/* if the container has the state */
bool have_state(States *s, State state) {
  for (size_t i = 0; i < s->len; ++i)
    if (s->states[i] == state)
      return true;
  return false;
}

/* if two containers have the same state */
bool have_same_state(States *s1, States *s2) {
  for (size_t i = 0; i < s1->len; ++i) {
    for (size_t j = 0; j < s2->len; ++j) {
      if (s1->states[i] == s2->states[j])
        return true;
    }
  }
  return false;
}

/*
 * ============================================================================
 * builder/lexer.c - Lexer for regular expressions
 * ============================================================================
 */

typedef enum TokenType { LPAREN, RPAREN, REPEAT, OR, LITERAL, END } TokenType;

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
  char *input;
  char *current_char;
  Token *current_token;
} Lexer;

/* create a new lexer from input string */
Lexer *new_lexer(char *input) {
  Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
  lexer->input = input;
  lexer->current_char = lexer->input;
  lexer->current_token = NULL;
  return lexer;
}

Token *get_next_token(Lexer *lexer) {
  if (lexer->current_token != NULL)
    free(lexer->current_token);

  char current_char = *lexer->current_char;
  if (current_char == '\0') {
    lexer->current_token = new_token(END, '\0');
  } else if (current_char == '(') {
    lexer->current_token = new_token(LPAREN, '(');
  } else if (current_char == ')') {
    lexer->current_token = new_token(RPAREN, ')');
  } else if (current_char == '|') {
    lexer->current_token = new_token(OR, '|');
  } else if (current_char == '*') {
    lexer->current_token = new_token(REPEAT, '*');
  } else {
    lexer->current_token = new_token(LITERAL, current_char);
  }
  ++(lexer->current_char);

  return lexer->current_token;
}

/*
 * ============================================================================
 * builder/parser.c - Parser for regular expressions
 * ============================================================================
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
 * factor := base ('*')*
 */
static Ast *parse_factor(Parser *parser) {
  Ast *node = parse_base(parser);
  while (parser->current_token->type == REPEAT) {
    eat(parser, REPEAT);
    node = new_ast_repeat(node);
  }
  return node;
}

/*
 * base := LITERAL | '(' expr ')'
 */
static Ast *parse_base(Parser *parser) {
  if (parser->current_token->type == LITERAL) {
    char value = parser->current_token->value;
    eat(parser, LITERAL);
    return new_ast_literal(value);
  } else if (parser->current_token->type == LPAREN) {
    eat(parser, LPAREN);
    Ast *node = parse_expr(parser);
    eat(parser, RPAREN);
    return new_ast_surround(node);
  } else {
    printf("unexpected trailing: %d\n", parser->current_token->type);
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

/*
 * ============================================================================
 * nfa.c - NFA (Non-deterministic Finite Automaton) implementation
 * ============================================================================
 */

char EPSILON = -1;

typedef struct Edge {
  char input;
  State from;
  State to;
} Edge;

typedef struct NFA {
  State states_count;
  States *target_states;
  Edge *edges[MAX];
  unsigned int edges_count;
} NFA;

/* create a new NFA */
NFA *new_nfa() {
  NFA *nfa = (NFA *)malloc(sizeof(NFA));
  nfa->states_count = 0;
  nfa->target_states = NULL;
  nfa->edges_count = 0;
  return nfa;
}

/* create a new edge with given input, from, to */
Edge *new_edge(char input, State from, State to) {
  Edge *e = (Edge *)malloc(sizeof(Edge));
  e->input = input;
  e->from = from;
  e->to = to;
  return e;
}

/* set the states count of an NFA */
void set_states_count(NFA *nfa, State states_count) {
  nfa->states_count = states_count;
}

/* set the target states of an NFA */
void set_target_states(NFA *nfa, States *s) { nfa->target_states = s; }

/* add an edge to an NFA */
void push_edge(NFA *nfa, Edge *e) {
  nfa->edges[nfa->edges_count] = e;
  ++(nfa->edges_count);
}

/* free an NFA */
void free_nfa(NFA *nfa) {
  /* free edges */
  for (size_t i = 0; i < nfa->edges_count; ++i) {
    free(nfa->edges[i]);
  }
  /* free target states */
  if (nfa->target_states != NULL) {
    free(nfa->target_states);
  }
  /* free nfa */
  free(nfa);
  nfa = NULL;
}

/* return all states reachable with epsilon lables from the given states */
States *epsilon_closure(NFA *nfa, States *s) {
  States *new_s = new_states();

  /* add all original states to the closure first */
  for (size_t i = 0; i < s->len; ++i) {
    push_state(new_s, s->states[i]);
  }

  /* simulate the original states as a stack */
  while (s->len > 0) {
    --(s->len);
    State state = s->states[s->len];
    for (size_t i = 0; i < nfa->edges_count; ++i) {
      Edge *e = nfa->edges[i];
      if (e->input == EPSILON && e->from == state) {
        State next_state = e->to;
        if (!have_state(new_s, next_state)) {
          push_state(new_s, next_state);
          s->states[s->len] = next_state;
          ++(s->len);
        }
      }
    }
  }

  free(s);
  return new_s;
}

/* return all states reachable with given input from the given states */
States *move(NFA *nfa, States *s, char input) {
  States *new_s = new_states();
  for (size_t i = 0; i < s->len; ++i) {
    for (size_t j = 0; j < nfa->edges_count; ++j) {
      Edge *e = nfa->edges[j];
      if (e->input == input && e->from == s->states[i]) {
        State next_state = e->to;
        if (!have_state(new_s, next_state)) {
          push_state(new_s, next_state);
        }
      }
    }
  }
  free(s);
  return new_s;
}

/* if the NFA matches the given input string */
bool match_string(NFA *nfa, char *input) {
  States *s = new_states();
  push_state(s, 0);
  s = epsilon_closure(nfa, s);

  char *next_char = input;
  while (*next_char != '\0') {
    s = epsilon_closure(nfa, move(nfa, s, *next_char));
    ++next_char;
  }
  bool result = have_same_state(s, nfa->target_states);
  free(s);
  return result;
}

/*
 * ============================================================================
 * builder.c - Build an NFA from a string
 * ============================================================================
 */

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
