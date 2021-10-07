grammar CoraxExpr;

// NOTE -- constant expressions will need to be identified through semantics
// TODO -- this needs to be consolidated as much as possible to avoid
// ridiculously long trees
              // Primary
expr_primary  : IDENTIFIER                         # identifier
              | INT                                # constInt
              | FLT                                # constFlt
              | STRING                             # string
              | '(' expression ')'                 # group
              // TODO -- actually, postfix needs to be separated from
              // unary, because something like this "func++()" could be
              // interpreted as a call
              // POSTFIX
              | expr_primary '[' expression ']'    # indexing
              | expr_primary '(' arg_expr_list? ')'# call // (go to page 70)
              | expr_primary '.' IDENTIFIER        # member
              | expr_primary '->' IDENTIFIER       # indirectMember
              | expr_primary '++'                  # incrementPost
              | expr_primary '--'                  # decrementPost
              // UNARY
              | '++' expr_primary                  # incrementUnary
              | '--' expr_primary                  # decrementUnary
              | '&' expr_cast                      # address
              | '*' expr_cast                      # dereference
              | '+' expr_cast                      # positive 
              | '-' expr_cast                      # negative
              | '~' expr_cast                      # not
              | '!' expr_cast                      # negation
              | 'sizeof' expr_primary              # sizeof
              | 'sizeof' '(' type_name ')'         # sizeofType
              ;

arg_expr_list : expr_assi (',' expr_assi)* ','?;

              // postfix
// expr_postfix  : expr_primary                       # exprPostfix         
//               | expr_postfix '[' expression ']'    # indexing
//               | expr_postfix '(' arg_expr_list ')' # call // (go to page 70)
//               | expr_postfix '.' IDENTIFIER        # member
//               | expr_postfix '->' IDENTIFIER       # indirectMember
//               | expr_postfix '++'                  # incrementPost
//               | expr_postfix '--'                  # decrementPost
//               // TODO -- add this after declaration / initialization is settled
//               // | '(' typename ')' '{' initializer ','? '}' #idk // (page 69)
//               ;
              
//               // Unary
// expr_unary    : expr_postfix                 # exprUnary
//               | '++' expr_unary              # incrementUnary
//               | '--' expr_unary              # decrementUnary
//               // TODO -- make these cast expressions!
//               | '&' expression               # address
//               | '*' expression               # dereference
//               | '+' expression               # positive 
//               | '-' expression               # negative
//               | '~' expression               # not
//               | '!' expression               # negation
//               | 'sizeof' expr_unary          # sizeof
//               | 'sizeof' '(' typename ')'    # sizeofType
//               ;

expr_cast     : expr_primary                  # exprCast
              | '(' type_name ')' expr_cast   # cast
              ;

              // ARITHMETIC
              // Multiplicative
expr_arith    : expr_cast                    # exprMulti
              | expr_arith '*' expr_arith    # mult
              | expr_arith '/' expr_arith    # div
              | expr_arith '%' expr_arith    # mod
              // Aditive
              | expr_arith '+' expr_arith    # plus
              | expr_arith '-' expr_arith    # minus
              // Bitwise shift
              | expr_arith '<<' expr_arith   # shiftLeft
              | expr_arith '>>' expr_arith   # shiftRight
              // Relational
              | expr_arith '<' expr_arith    # less
              | expr_arith '>' expr_arith    # greater
              | expr_arith '<=' expr_arith   # less_equal
              | expr_arith '>=' expr_arith   # greater_equal
              // Equality
              | expr_arith '==' expr_arith   # equal
              | expr_arith '!=' expr_arith   # notEqual
              // Bitwise
              | expr_arith '&' expr_arith    # bit_and
              | expr_arith '^' expr_arith    # bit_xor
              | expr_arith '|' expr_arith    # bit_or
              // Logical
              | expr_arith '&&' expr_arith   # and
              | expr_arith '||' expr_arith   # or
              ;

              // Ternary
expr_tern     : expr_arith                              # exprTern
              | expr_arith '?' expression ':' expr_tern # ternary
              ;

expr_assi     : expr_tern                     # exprAssi
              | expr_primary '=' expr_assi    # assignment
              | expr_primary '*=' expr_assi   # assignmentMult
              | expr_primary '/=' expr_assi   # assignmentDiv
              | expr_primary '%=' expr_assi   # assignmentMod
              | expr_primary '+=' expr_assi   # assignmentPlus
              | expr_primary '-=' expr_assi   # assignmentMinus
              | expr_primary '<<=' expr_assi  # assignmentShiftLeft
              | expr_primary '>>=' expr_assi  # assignmentShiftRight
              | expr_primary '&=' expr_assi   # assignmentBitAnd
              | expr_primary '^=' expr_assi   # assignmentBitXor
              | expr_primary '|=' expr_assi   # assignmentBitOr
              ;

              // Comma
expression    : expr_assi                    # exprExpression
              | expression ',' expression    # comma
              ;

expr_const    : expr_tern;

type_name      : IDENTIFIER+;

STRING        : '"' (~["\r\n] | '""' | '\\"')* '"';
INT           : [1-9][0-9_]* | '0';
FLT           : ([1-9][0-9_]* | '0') '.' ([1-9][0-9_]* | '0');
IDENTIFIER    : [a-zA-Z_][a-zA-Z_0-9]*;
