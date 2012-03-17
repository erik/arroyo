# Grammar
In progress grammar for the language (not finalized).

*written in pseudo BNF form*

```
ID		:= [a-zA-Z_][a-zA-Z0-9_]+
PRIMITIVE	:= FUNCTION | STRING | REAL | BOOLEAN | ARRAY | HASH

BOOLEAN		:= true | false
ARRAY		:= "[" EXPRESSION* "]"
HASH		:= "{" (PRIMITIVE ":" EXPRESSION)* "}"

FUNCTION	:= "fn" "(" (ID (":" ID)? )* ")" EXPRESSION

STRING		:= "\"" [^"\n]* "\""
REAL		:= [0-9]+(\.[0-9]+)?

FOR		:= "for" ID "<-" EXPRESSION "to" EXPRESSION ("by" EXPRESSION)?  EXPRESSION
WHILE           := "while" EXPRESSION EXPRESSION
LOOP		:= FOR | WHILE

IF		:= "if" EXPRESSION EXPRESSION ("elseif" EXPRESSION EXPRESSION)* ("else" EXPRESSION)?
WHEN		:= "when" EXPRESSION EXPRESSION

CONDITIONAL	:= ">" | "<" | ">=" | "<=" | "=" | "/="

CONDITIONALEXP  := EXPRESSION CONDITIONAL EXPRESSION
BLOCK		:= "(" EXPRESSION* ")"
ASSIGNMENT	:= ID "<-" EXPRESSION

EXPRESSION	:= ID | PRIMITVE | CONDITIONALEXP | LOOP | BLOCK | ASSIGNMENT

PROGRAM		:= EXPRESSION* EOS
```
