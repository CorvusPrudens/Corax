grammar PreExpr;

statement     : expression NEWLINE;

expression    : DEFINED NAME                        # defined
              | DEFINED LP NAME RP                  # defined
              | EXCL expression                     # negation 
              | PLUS expression                     # positive
              | MINUS expression                    # negative
              | NOT expression                      # not
              | expression MULT expression          # mult
              | expression DIV expression           # div
              | expression MOD expression           # mod
              | expression PLUS expression          # plus
              | expression MINUS expression         # minus
              | expression SHIFT_LEFT expression    # shift_left
              | expression SHIFT_RIGHT expression   # shift_right
              | expression LESS expression          # less
              | expression GREATER expression       # greater
              | expression LESS_EQUAL expression    # less_equal
              | expression GREATER_EQUAL expression # greater_equal
              | expression EQUAL expression         # equal
              | expression NOT_EQUAL expression     # not_equal
              | expression BIT_AND expression       # bit_and
              | expression BIT_XOR expression       # bit_xor
              | expression BIT_OR expression        # bit_or
              | expression AND expression           # and
              | expression OR expression            # or
              | number                              # const
              | LP expression RP                    # group
              ;

number        : DEC #dec
              | BIN #bin
              | HEX #hex
              | OCT #oct
              | FLT #flt
              | SCI #flt
              ;

// lexer

DEFINED       : 'defined';

LP            : '(';
RP            : ')';

ESCAPED_NEW   : '\\' '\r'? '\n' -> skip;
NEWLINE       : '\r'? '\n';

// We shouldn't ever see this
// NEWLINE       : '\r'? '\n' -> mode(DEFAULT_MODE);

STRING        : '"' (~["\r\n] | '""' | '\\"')* '"';
LIBRARY       : '<' .+? '>';
COMMENT       : '//' ~[\n\r]* [\n\r] -> skip;
COMMENT_BLOCK : '/*' .*? '*/' -> skip;

NAME          : [a-zA-Z_][a-zA-Z_0-9]*;

DOUBLE_HASH   : '##';
HASH          : '#';
EXCL          : '!';
PLUS          : '+';
MINUS         : '-';
NOT           : '~';
MULT          : '*';
DIV           : '/';
MOD           : '%';
SHIFT_LEFT    : '<<';
SHIFT_RIGHT   : '>>';
LESS          : '<';
GREATER       : '>';
LESS_EQUAL    : '<=';
GREATER_EQUAL : '>=';
EQUAL         : '==';
NOT_EQUAL     : '!=';
AND           : '&&';
OR            : '||';
BIT_AND       : '&';
BIT_XOR       : '^';
BIT_OR        : '|';

DEC           : [1-9][0-9_]* | '0';
HEX           : '0x'[0-9A-Fa-f][0-9A-Fa-f_]*;
BIN           : '0b'[0-1][0-1_]*;
OCT           : '0'[0-7]+;
FLT           : ([1-9][0-9_]* | '0') '.' ([1-9][0-9_]* | '0');

fragment FD   : ([1-9][0-9]* | '0') '.'?
              | ([1-9][0-9]* | '0') '.' ([1-9][0-9]* | '0')
              | '.' ([1-9][0-9]* | '0')
              ;

SCI           : FD 'e' '-'? [0-9]+;

WHITESPACE    : [ \t]+ -> channel(HIDDEN);