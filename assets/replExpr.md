

## Grammar

### Expression

> *note*: the `powershell` in the code block was just for syntax highlighting purpose.

```powershell
expression     -> assignment ;

assignment     -> IDENTIFIER "=" assignment
                | logic_or ;

logic_or       -> logic_and ( "or" logic_and )* ;
logic_and      -> equality ( "and" equality )* ; 
equality       -> comparison ( ( "!=" | "==" ) comparison )* ;
comparison     -> term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
term           -> factor ( ( "-" | "+" ) factor )* ;
factor         -> unary ( ( "/" | "*" ) unary )* ;
unary          -> ( "!" | "-" | "*" ) unary 
                | call ;
call           -> primary ( "(" arguments? ")" )* ;

primary        -> NUMBER | STRING | "true" | "false" | "nil"
                | "(" expression ")" | IDENTIFIER ;
```

### Miscellaneous
```cpp
arguments     -> expression ( "," expression )* ;
function      -> IDENTIFIER "(" parameters? ")" block ;
parameters    -> IDENTIFIER ( "," IDENTIFIER )* ;
alnums        -> [a-zA-Z0-9] 
               | [...] ;
```

### Lexical
```cpp
number        -> digit + ( "." digit + )? ;
string        -> "\"" + ([[alnums]])* + "\"" ;
identifier    -> [a-zA-Z_] + [a-zA-Z0-9_]* ;
```