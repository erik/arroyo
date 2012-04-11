# Grammar
In progress grammar for the language (not finalized).

**Warning** this grammar is based on the syntax as of git commit
  92c1398d4, and will probably be out of date quickly.

*written in pseudo BNF form*

```
ID             := [a-zA-Z_][a-zA-Z0-9_]+
PRIMARY        := FUNCTION | STRING | REAL | BOOLEAN | ARRAY | HASH | ID | NIL

BOOLEAN        := "true" | "false"
ARRAY          := "[" EXPRESSION* "]"
NIL            := "nil"
FUNCTION       := "fn" ID? "(" (ID (":" ID)? )* ")" EXPRESSION
STRING         := "\"" [^"\n]* "\""
REAL           := [0-9]+(\.[0-9]+)?

LOOP           := "loop" EXPRESSION? "while" | "until" | "do" EXPRESSION
IF             := "if" EXPRESSION EXPRESSION ("elseif" EXPRESSION EXPRESSION)* ("else" EXPRESSION)?
CASE           := "case" EXPRESSION "of" "(" (EXPRESSION "=>" EXPRESSION)* ("default" "=>" EXPRESSION)? ")"

BINARY         := "+" | "-" | "*" | "/" | ">" | "<" | ">=" | "<=" | "=" | "/="
UNARY          := "-" | "++" | "print" | "!" | "#"

UNARYEXP       := UNARY EXPRESSION
BINARYEXP      := EXPRESSION BINARY EXPRESSION
BLOCK          := "(" EXPRESSION* ")"
ASSIGNMENT     := ID "<-" EXPRESSION

EXPRESSION     := PRIMARY | BINARYEXP | UNARYEXP | LOOP | IF | CASE | BLOCK | ASSIGNMENT

PROGRAM        := EXPRESSION* EOS
```
