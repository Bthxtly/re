#! /usr/bin/bash

target_file="re_lib.c"

cat >$target_file <<EOF
/*
 * Merged regex library
 * For embedding into lers projects
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
EOF

cat >>$target_file <<EOF

/*
 * ============================================================================
 * state.c - State management
 * ============================================================================
 */
EOF

cat src/state.c >>$target_file

cat >>$target_file <<EOF

/*
 * ============================================================================
 * builder/lexer.c - Lexer for regular expressions
 * ============================================================================
 */
EOF

cat src/builder/lexer.c >>$target_file

cat >>$target_file <<EOF

/*
 * ============================================================================
 * builder/ast.c - Abstract Syntax Tree
 * ============================================================================
 */
EOF

cat src/builder/ast.c >>$target_file

cat >>$target_file <<EOF

/*
 * ============================================================================
 * builder/parser.c - Parser for regular expressions
 * ============================================================================
 */
EOF

cat src/builder/parser.c >>$target_file

cat >>$target_file <<EOF

/*
 * ============================================================================
 * edge.c - Edges between states
 * ============================================================================
 */
EOF

cat src/edge.c >>$target_file

cat >>$target_file <<EOF

/*
 * ============================================================================
 * nfa.c - NFA (Non-deterministic Finite Automaton) implementation
 * ============================================================================
 */
EOF

cat src/nfa.c >>$target_file

cat >>$target_file <<EOF

/*
 * ============================================================================
 * builder.c - Build an NFA from a string
 * ============================================================================
 */
EOF

cat src/builder.c >>$target_file

cat >>$target_file <<EOF

/*
 * ============================================================================
 * match.c - Functions to match string with patterns
 * ============================================================================
 */
EOF

cat src/match.c >>$target_file

# remove `#include`s from source codes
sed -i '10,${/#include/d}' $target_file

# fix `#include "util/vector.c"`
sed -i '/#define TYPE/{
  r src/util/vector.c
}' $target_file
