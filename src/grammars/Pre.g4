grammar Pre;

parse         : anything* EOF?;

anything      : directive      #topDirective
              | anything_else+ #topAny
              ;

directive     : include_
              | if_
              | ifdef_
              | ifndef_ 
              | define_ 
              | undef_ 
              | line_
              | error_
              | empty_
              ;

include_      : INCLUDE STRING NEWLINE  #string
              | INCLUDE LIBRARY NEWLINE #library
              ;

if_           : IF anything_expr NEWLINE anything* elif_* else_? endif_;

elif_         : ELIF anything_expr NEWLINE anything*;

ifdef_        : IFDEF NAME NEWLINE anything* else_? endif_;

ifndef_       : IFNDEF NAME NEWLINE anything* else_? endif_;

else_         : ELSE NEWLINE anything*;
 
endif_        : ENDIF end;

define_       : DEFINE NAME anything_expr? end                                 #object
              | DEFINE NAME LP NAME (COMMA NAME)* COMMA? RP anything_expr? end #function
              ;

undef_        : UNDEF NAME end;

line_         : LINE number end;

error_        : ERROR STRING end;

pragma_       : PRAGMA anything* end;

empty_        : HASH end;

// These should be converted into simply
// decimal ints and floats for easy compilation
number        : DEC #dec
              | BIN #bin
              | HEX #hex
              | OCT #oct
              | FLT #flt
              | SCI #sci
              ;

anything_expr : anything_opt+;

anything_opt  : DEFINED NAME        #anyPass
              | DEFINED LP NAME RP  #anyPass
              | NAME                #anyName
              | LP                  #anyPass
              | RP                  #anyPass
              | PP                  #anyPass
              | number              #anyNum
              | STRING              #anyPass
              | HASH                #anyPass
              | HASH_DOUBLE         #anyPass
              | COMMA               #anyPass
              ;

anything_else : anything_expr?  NEWLINE    #anyNewline
              | anything_expr              #anyEof
              ; 

end           : NEWLINE | EOF;


// lexy
ESCAPED_NEW   : '\\' '\r'? '\n' -> skip;
NEWLINE       : '\r'? '\n';

STRING        : '"' (~["\r\n] | '""' | '\\"')* '"';
LIBRARY       : '<' [a-zA-Z_0-9./\-]+ '>';
COMMENT       : '//' ~[\n\r]* [\n\r] -> channel(HIDDEN);
COMMENT_BLOCK : '/*' .*? '*/' -> channel(HIDDEN);
TEST_BLOCK    : '$' .*? '$end' -> skip;

INCLUDE       : '#include';
IF            : '#if';
IFDEF         : '#ifdef';
IFNDEF        : '#ifndef';
ELSE          : '#else';
ENDIF         : '#endif';
DEFINE        : '#define';
ERROR         : '#error';
ELIF          : '#elif';
UNDEF         : '#undef';
LINE          : '#line';
PRAGMA        : '#pragma';
HASH          : '#';
HASH_DOUBLE   : '##';
DEFINED       : 'defined';
LP            : '(';
RP            : ')';
COMMA         : ',';

NAME          : [a-zA-Z_][a-zA-Z_0-9]*;

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
PP            : .+?;