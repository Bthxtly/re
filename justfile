# https://just.systems

default: merge

run:
  @gcc src/main.c
  @./a.out
  @rm a.out

# merge source files into one file to be embedded into other projects
merge: test # test before merge
  @./merge.sh

# tests
test_builder:
  @gcc test/builder.c
  @./a.out
  @rm a.out

test_nfa:
  @gcc test/nfa.c
  @./a.out
  @rm a.out

test_match:
  @gcc test/match.c
  @./a.out
  @rm a.out

test: test_builder test_nfa test_match
