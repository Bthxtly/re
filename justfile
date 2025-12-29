# https://just.systems

run:
  @gcc src/main.c
  @./a.out
  @rm a.out

test_nfa:
  @gcc test/nfa.c
  @./a.out
  @rm a.out

test_builder:
  @gcc test/builder.c
  @./a.out
  @rm a.out

test: test_nfa test_builder

default:
  echo 'Hello, world!'
