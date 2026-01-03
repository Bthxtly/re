# re
This is a very simple regular expression engine written in pure C.

The project is here for [my implementation](https://github.com/bthxtly/lers) of
[Lex](https://en.wikipedia.org/wiki/Lex_(software)), which generates lexical analyzers.

## Feature
Support some regular expression rules:

| Meta-character(s)            | Description |
| :--------------------------: | :---------: |
| r<sub>1</sub>r<sub>2</sub>   | And         |
| r<sub>1</sub>\|r<sub>2</sub> | Or          |
| r*                           | Repeat      |
| +*                           | One Or More |
| \(r\)                        | Group       |
| .                            | Dot         |
| \[^abc0-9\]                  | Range       |

## Usage
Refer to [this test file](test/match.c).
