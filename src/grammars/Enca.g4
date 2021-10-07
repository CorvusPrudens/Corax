grammar Enca;

parse       : statement*;

statement   : instruction  # statInstr
            | data         # statData
            | label        # statLab
            ;

instruction : mnemonic                      # instr
            | mnemonic operands             # instrOper
            | mnemonic conditional          # instrCond
            | mnemonic operands conditional # instrOperCond
            ;

conditional : '->' condition;

condition   : 'zero'
            | 'carry'
            | 'negative'
            | 'equal'
            | 'greater'
            | 'less'
            ;

label       : NAME ':';

// a bit annoying to specify newlines, but it's only for this so...
data        : storage  '{'  data_list?  '}' # dataArray
            | storage data_element         # dataSingle
            ;

storage     : NAME dimension specifier_list?;

dimension   : '[' ']'        # dimEmpty
            | '[' number ']' # dimNumber
            ;

specifier_list : '(' specifier (',' specifier)* ','? ')';

specifier   : NAME ('=' number)?;

data_list   : data_element  (','  data_element )* ','?;

data_element : number   # dataNumber
             | variable # dataVariable
             ;

operands    : operand (',' operand)*;

operand     : '[' operand (',' number)? ']' # opRel
            | reg                           # opReg
            | number                        # opNum
            | condition                     # opCond
            | variable                      # opVar
            ;

reg         : REGISTER # regStandard
            | STACK    # regStack
            | BASE     # regBase
            ;

number      : DEC # numDec
            | FLT # numFlt
            | SCI # numSci
            | HEX # numHex
            | BIN # numBin
            | OCT # numOct
            ;

mnemonic    : MNEMONIC;

variable    : NAME     # var
            | AMP NAME # varAddr
            ;

// lexer

COMMENT       : '//' ~[\n\r]* ([\n\r] | EOF) -> skip;
COMMENT_BLOCK : '/*' .*? '*/' -> skip;
// NEWLINE       : '\r'? '\n';

REGISTER      : 'r' ([1-9][0-9_]* | '0');
STACK         : 'sp';
BASE          : 'bp';

MNEMONIC      : 'nop' | 'ldr' | 'str' | 'cmp'
              | 'cps' | 'add' | 'sub' | 'mul'
              | 'div' | 'mod' | 'and' | 'or'
              | 'xor' | 'not' | 'lsl' | 'lsr'
              | 'jmp' | 'push'| 'pop'
              ;

NAME          : [a-zA-Z_][a-zA-Z_0-9]*;

DEC           : '-'? [1-9][0-9_]* | '0';
HEX           : '-'? '0x'[0-9A-Fa-f][0-9A-Fa-f_]*;
BIN           : '-'? '0b'[0-1][0-1_]*;
OCT           : '-'? '0'[0-7]+;
FLT           : '-'? ([1-9][0-9_]* | '0') '.' ([1-9][0-9_]* | '0');

AMP           : '&';

fragment FD   : ([1-9][0-9]* | '0') '.'?
              | ([1-9][0-9]* | '0') '.' ([1-9][0-9]* | '0')
              | '.' ([1-9][0-9]* | '0')
              ;

SCI           : '-'? FD 'e' '-'? [0-9]+;

WHITESPACE    : [ \t\n\r] -> channel(HIDDEN);