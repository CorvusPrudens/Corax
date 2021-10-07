
#ifndef ENCA_OPERAND_H
#define ENCA_OPERAND_H

#include <string>
#include "symbols.h"
#include "number.h"

using std::string;


class Operand {

  public:

    enum Class {
      REGISTER = 0,
      NUMBER,
      CONDITION,
      NAME,

      REGISTER_REL,
      NUMBER_REL,
      CONDITION_REL,
      NAME_REL,

      NONE,

      CLASS_COUNT,
    };

    Operand() {}
    virtual ~Operand() {}

    virtual Class getClass() = 0;
    virtual string getIdentifier() { throw 1; }
    virtual uint16_t getValue() = 0;

    void setRelative(int16_t offset = 0);

    bool isRelative = false;
    int16_t relativeOffset;

};

class RegisterOp : public Operand {
  
  public:

    enum Index {
      R0 = 0,
      R1,
      R2,
      R3,
      R4,
      SP,
      BP,
      PC,
      REG_COUNT,
    };

    inline static string Names[] = {
      "r0",
      "r1",
      "r2",
      "r3",
      "r4",
      "sp",
      "bp",
      "pc",
    };

    RegisterOp(string name);
    ~RegisterOp() {}

    Class getClass() override { return isRelative ? REGISTER_REL : REGISTER; }
    uint16_t getValue() override { return (uint16_t) index; }

    Index index;
};

class NumberOp : public Operand {
  
  public:

    NumberOp(Number n) { number = n; }
    ~NumberOp() {}

    Class getClass() override { return isRelative ? NUMBER_REL : NUMBER; }

    /** Truncates whatever bits were input into a 16 bit word
     * 
     */
    uint16_t getValue() override { return number.getValue(); }

    Number number;
};

class ConditionOp : public Operand {
  
  public:

    enum Cond {
      ZERO,
      CARRY,
      NEGATIVE,
      EQUAL,
      GREATER,
      LESS,
      COND_COUNT,
    };

    inline static string Names[] = {
      "zero",
      "carry",
      "negative",
      "equal",
      "greater",
      "less",
    };

    ConditionOp() {}
    ConditionOp(string c);
    ~ConditionOp() {}

    Class getClass() override { return isRelative ? CONDITION_REL : CONDITION; }
    uint16_t getValue() override;

    Cond condition;
};

class VariableOp : public Operand {
  
  public:

    VariableOp(string n, SymbolTable* t, bool addr) { name = n; table = t; isAddress = addr; }
    ~VariableOp() {}

    Class getClass() override;
    string getIdentifier() override { return name; }
    // Not quite sure what to do here (maybe have pointer to class with more details?)
    uint16_t getValue() override;

    string name;
    SymbolTable* table;
    bool isAddress;
};

#endif