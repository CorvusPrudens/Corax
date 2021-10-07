grammar Corax;

import CoraxExpr;

// parsey

// This represents the translation unit
parse       : extern_decl+;

// TODO -- ensure this is all refactored into antlr-idiomatic code
// NOTE -- the rule says init_decl is optional, but that doesn't make sense to me
// NOTE -- this ^ is because structs and such can have an optional name after the {}!!!
// TODO -- add predicate or something to make this work (probably to decl_spec)
declaration    : decl_spec init_decl_list ';';

init_decl_list : init_decl (',' init_decl)* ','?;

// Declaration specifiers
decl_spec   : decl_spec_item+;

decl_spec_item : storage_spec # storageSpecifier
               | type_spec    # typeSpecifier
               | type_qual    # typeQualifier
               | 'inline'     # funcSpecifier
               ;

init_decl   : declarator                  # initDeclUnassigned
            | declarator '=' initializer  # initDeclAssigned
            ;

storage_spec : 'typedef'
             | 'extern'
             | 'static'
             | 'auto'
             | 'register'
             ;

// this kind of stuff should (probably?) be done with semantics
type_spec    : 'void'        # typeStd
             | 'char'        # typeStd
             | 'short'       # typeStd
             | 'int'         # typeStd
             | 'long'        # typeStd
             | 'float'       # typeStd
             | 'double'      # typeStd
             | 'signed'      # typeStd
             | 'unsigned'    # typeStd
             | '_Bool'       # typeStd
             | '_Complex'    # typeStd
             | struct_union  # typeStructUnion
             | enum_spec     # typeEnum
             | IDENTIFIER    # typeTypedef
             ;

struct_union : 'struct' IDENTIFIER? OBRACE struct_decl CBRACE # structDefined
             | 'struct' IDENTIFIER                      # structDeclared
             | 'union' IDENTIFIER? OBRACE struct_decl CBRACE  # unionDefined
             | 'union' IDENTIFIER                       # unionDeclared
             ;

struct_decl : spec_qual_list struct_declr_list ';';

// spec_qual_list : type_spec spec_qual_list? | type_qual spec_qual_list?;
spec_qual_list : (type_spec | type_qual)+;

// struct_declr_list : struct_declr | struct_declr_list ',' struct_declr;
struct_declr_list : struct_declr (',' struct_decl)* ','?;

struct_declr : declarator 
             | declarator? ':' expr_const
             ;

enum_spec    : 'enum' IDENTIFIER? OBRACE enumerator (',' enumerator)* ','? CBRACE # enumDefined
             | 'enum' IDENTIFIER                                            # enumDeclared
             ;

enumerator   : IDENTIFIER                 
             | IDENTIFIER '=' expr_const
             ;

type_qual    : 'const'    # qualConst
             | 'restrict' # qualRestrict
             | 'volatile' # qualVolatile
             ;

declarator   : pointer? direct_decl;

direct_decl  : IDENTIFIER                                              # dirId
             | '(' declarator ')'                                      # dirParen // I don't know what this means
             | direct_decl '[' type_qual* expr_assi? ']'               # dirDim
             | direct_decl '[' 'static' type_qual* expr_assi ']'       # dirDimStatic
             | direct_decl '[' type_qual+ 'static' expr_assi ']'       # dirDimStatic
             | direct_decl '[' type_qual* '*' ']'                      # dirDimVar
             | direct_decl '(' param_type_list? ')'                    # dirFunc
             | direct_decl '(' IDENTIFIER (',' IDENTIFIER)* ','? ')'   # dirFuncID
             ;

pointer : pointer_item+
        // | '*' type_qual* pointer
        ;

pointer_item : '*' type_qual*;

param_type_list: param_list ','?       # paramList
               | param_list ',' '...'  # paramListElipse
               | 'void'                # ParamListEmpty
               ;

param_list: param_decl (',' param_decl)*;

param_decl : decl_spec declarator     # paramDecl
           | decl_spec abstract_decl? # paramAbst
           ;

type_name      : spec_qual_list abstract_decl?;

// abstract_decl : pointer 
//               | pointer? direct_abstr
//               ;

// abstract_decl : pointer
//               | pointer abstract_decl
//               | '(' abstract_decl ')'
//               | abstract_single+
//               ;

// abstract_single : '[' type_qual* expr_assi? ']'
//                 | '[' 'static' type_qual* expr_assi ']'
//                 | '[' type_qual* 'static' expr_assi ']'
//                 | '[' '*' ']'
//                 | '(' param_type_list ')'
//                 ;

// NOTE -- this may not totally capture the
// rules, since they tend towards mutual left-recursion
abstract_decl : pointer
              | pointer abstract_decl
              | '(' abstract_decl ')' 
              | abstract_decl '[' type_qual* expr_assi? ']'
              | abstract_decl '[' 'static' type_qual* expr_assi ']'
              | abstract_decl '[' type_qual* 'static' expr_assi ']'
              | abstract_decl '[' '*' ']'
              | abstract_decl '(' param_type_list ')'
              ;

initializer   : expr_assi          # initAssign
              | OBRACE init_list CBRACE  # initList
              ;

init_list     : designation? initializer (',' designation? initializer)* ','?;

designation   : designator+ '=';

designator    : '[' expr_const ']'
              | '.' IDENTIFIER
              ;

// TODO -- this should probably go into one big rule with labels
// statement     : stat_labeled
//               | stat_compound
//               | stat_expr
//               | stat_select
//               | stat_iter
//               | stat_jump
//               ;

// TODO -- an item like:
// uart(arg);
// Can be seen as a declaration or expression. 
// This needs to be resolved properly!!
block_item    : statement   # blockStat
              | declaration # blockDecl
              ;

// TODO -- label these
                // Labeled statement
statement     : IDENTIFIER ':' statement                                              # statLabeled
              | 'case' expr_const ':' statement                                       # statCase
              | 'default' ':' statement                                               # statDefault
              // Compound statement
              | stat_compound                                                         # statCompound
              // Expression statement
              | expression? ';'                                                       # statExpr
              // Selection statement
              | 'if' '(' expression ')' statement                                     # statIf
              | 'if' '(' expression ')' statement 'else' statement                    # statIfElse
              | 'switch' '(' expression ')' statement                                 # statSwitch
              // Iteration statement
              | 'while' '(' expression ')' statement                                  # statWhile
              | 'do' statement 'while' '(' expression ')' ';'                         # statDoWhile
              | 'for' '(' expression? ';' expression? ';' expression? ')' statement   # statFor
              | 'for' '(' declaration expression? ';' expression? ')' statement       # statFor2
              // Jump statement
              | 'goto' IDENTIFIER ';'                                                 # statGoto
              | 'continue' ';'                                                        # statContinue
              | 'break' ';'                                                           # statBreak
              | 'return' expression? ';'                                              # statReturn
              ;

stat_compound : OBRACE block_item* CBRACE;

// stat_labeled  : IDENTIFIER ':' statement
//               | 'case' expr_const ':' statement
//               | 'default' ':' statement
//               ;

// stat_compound : OBRACE block_item* CBRACE;

// block_item    : declaration
//               | statement
//               ;

// stat_expr     : expression? ';';

// stat_select   : 'if' '(' expression ')' statement
//               | 'if' '(' expression ')' statement 'else' statement
//               | 'switch' '(' expression ')' statement
//               ;

// stat_iter     : 'while' '(' expression ')' statement
//               | 'do' statement 'while' '(' expression ')' ';'
//               | 'for' '(' expression? ';' expression? ';' expression? ')' statement
//               | 'for' '(' declaration expression? ';' expression? ')' statement
//               ;

// stat_jump     : 'goto' IDENTIFIER ';'
//               | 'continue' ';'
//               | 'break' ';'
//               | 'return' expression? ';'
        //       ;

extern_decl   : func_def     # topFunc
              | declaration  # topDecl
              ;

func_def      : decl_spec declarator declaration* stat_compound;

// lexy

STRING                 : '"' (~["\r\n] | '""' | '\\"')* '"';
CHAR                   : '\'' (('\\'.)|~[\\]) '\'';
COMMENT                : '//' ~[\n\r]* [\n\r] -> skip;
COMMENT_BLOCK          : '/*' .*? '*/' -> skip;
WHITESPACE             : [ \t\n\r] -> skip;
OBRACE                 : '{';
CBRACE                 : '}';