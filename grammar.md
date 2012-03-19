# Grammar
In progress grammar for the language (not finalized).

*written in pseudo BNF form*

```
ID             := [a-zA-Z_][a-zA-Z0-9_]+
PRIMARY        := FUNCTION | STRING | REAL | BOOLEAN | ARRAY | HASH | ID

BOOLEAN        := "true" | "false"
ARRAY          := "[" EXPRESSION* "]"
HASH           := "{" (STRING | ID ":" EXPRESSION)* "}"

FUNCTION       := "fn" ID? "(" (ID (":" ID)? )* ")" EXPRESSION

STRING         := "\"" [^"\n]* "\""
REAL           := [0-9]+(\.[0-9]+)?

LOOP           := "loop" EXPRESSION? "while" | "until" | "do" EXPRESSION

IF             := "if" EXPRESSION EXPRESSION ("elseif" EXPRESSION EXPRESSION)* ("else" EXPRESSION)?
WHEN           := "when" EXPRESSION EXPRESSION

CONDITIONAL    := ">" | "<" | ">=" | "<=" | "=" | "/="

CONDITIONALEXP := EXPRESSION CONDITIONAL EXPRESSION
BLOCK          := "(" EXPRESSION* ")"
ASSIGNMENT     := ID "<-" EXPRESSION

EXPRESSION     := PRIMARY | CONDITIONALEXP | LOOP | BLOCK | ASSIGNMENT

PROGRAM        := EXPRESSION* EOS
```
