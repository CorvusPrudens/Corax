
#include "compiler.h"

class OperatorBase {
  public:

    OperatorBase(ParserRuleContext* c, SymbolTable* t, Identifier* f, Compiler* co) {
      ctx = c;
      table = t;
      func = f;
      comp = co;
    }

    virtual Instruction::Abstr getAbstr() { throw 1; } // this should always be overriden
    virtual Instruction::Cond getCond() { return Instruction::Cond::EQUAL; } // this is only important for conditional expressions
    virtual bool validType1(Type& t) { return true; }
    virtual bool validType2(Type& t) { return true; }

    // TODO this can be much improved -- currently it eats up any temp vars it passes while looking for matching ones
    virtual Identifier& manageTemps(Type& t) {
      Identifier ass;
      if (func->funcTable->total_temps == 0) 
      {
        // This ensures unique names, since $var is not valid C
        string tempname = "$temp_" + std::to_string(table->temp_vars);
        func->funcTable->total_temps++;
        func->funcTable->temp_vars++;
        ass.name = tempname;
        ass.dataType = t;
        func->funcTable->AddSymbol(ass);
        return func->funcTable->GetLast();
      }
      else
      {
        for (int i = func->funcTable->temp_vars; i < func->funcTable->total_temps; i++) {
          string tempname = "$temp_" + std::to_string(i);

          try {
            Identifier& ti = func->funcTable->GetLocalSymbol(tempname);
            if (ti.dataType == t) {
              func->funcTable->temp_vars = i + 1;
              return ti;
            }
          } catch (int e) {
            // pass
          }
        }

        string tempname = "$temp_" + std::to_string(func->funcTable->total_temps);
        func->funcTable->total_temps++;
        func->funcTable->temp_vars = func->funcTable->total_temps;
        ass.name = tempname;
        ass.dataType = t;
        func->funcTable->AddSymbol(ass);
        return func->funcTable->GetLast();
      }
    }

    /////////////////////////////////////////
    // Binary expressions
    /////////////////////////////////////////

    virtual void perform(long double v1, long double v2, Result& res) { throw long_double_; }
    virtual void perform(double v1, double v2, Result& res) { throw double_; }
    virtual void perform(float v1, float v2, Result& res) { throw float_; }
    virtual void perform(unsigned long long v1, unsigned long long v2, Result& res) { throw unsigned_long_long_; }
    virtual void perform(long long v1, long long v2, Result& res) { throw long_long_; }
    virtual void perform(unsigned long v1, unsigned long v2, Result& res) { throw unsigned_long_; }
    virtual void perform(long v1, long v2, Result& res) { throw long_; }
    virtual void perform(unsigned v1, unsigned v2, Result& res) { throw unsigned_; }
    virtual void perform(int v1, int v2, Result& res) { throw int_; }
    virtual void perform(unsigned short v1, unsigned short v2, Result& res) { throw short_; }
    virtual void perform(unsigned char v1, unsigned char v2, Result& res) { throw unsigned_char_; }
    virtual void perform(signed char v1, signed char v2, Result& res) { throw signed_char_; }
    virtual void perform(char v1, char v2, Result& res) { throw char_; }

    // Currently, in an operation between an lvalue and constant, the
    // constant is always converted to the lvalue's type (which may
    // not always be desirable?)
    virtual void perform(Identifier& id, Result& op2, Result& res) {

      if (!validType1(id.dataType)) throw id.dataType;
      if (!validType2(op2.type)) throw op2.type;

      int priority1 = fetchPriority(id.dataType);
      int priority2 = fetchPriority(op2.type);

      Result lhs;
      lhs.setValue(id);

      // string tempname = "__temp_var_" + std::to_string(table->temp_vars++) + "__";
      // Identifier ass;
      // ass.name = tempname;

      Identifier& ass = manageTemps(lhs.id->dataType);

      if (priority1 == priority2) {
        func->function.add(Instruction(ctx, getAbstr(), getCond(), lhs, op2, ass));
      }
      else {
        op2.to(id.dataType);
        func->function.add(Instruction(ctx, getAbstr(), getCond(), lhs, op2, ass));
      }
      // else if (priority1 < priority2) {
      //   op2.to(id.dataType);
      //   ass.dataType = lhs.id.dataType;
      //   table->AddSymbol(ass);
      //   func->function.add(Instruction(ctx, getAbstr(), lhs, op2, ass));
      // }
      // else
      // {
      //   ass.dataType = op2.type;
      //   Result temp;
      //   temp.setValue(ass);

      //   func->function.add(Instruction(ctx, Instruction::Abstr::CONVERT, lhs, op2, ass));
      //   func->function.add(Instruction(ctx, getAbstr(), temp, op2, ass));
      // }
      res.setValue(ass);
    }
    virtual void perform(Result& op1, Identifier& id, Result& res) {

      if (!validType1(op1.type)) throw op1.type;
      if (!validType2(id.dataType)) throw id.dataType;

      int priority2 = fetchPriority(id.dataType);
      int priority1 = fetchPriority(op1.type);

      Result rhs;
      rhs.setValue(id);

      // string tempname = "__temp_var_" + std::to_string(table->temp_vars++) + "__";
      // Identifier ass;
      // ass.name = tempname;
      Identifier* ass;

      if (priority1 == priority2) {
        ass = &manageTemps(rhs.id->dataType);
        func->function.add(Instruction(ctx, getAbstr(), getCond(), op1, rhs, *ass));        
      }
      else {
        op1.to(id.dataType);
        ass = &manageTemps(rhs.id->dataType);
        func->function.add(Instruction(ctx, getAbstr(), getCond(), op1, rhs, *ass));
      }
      // else if (priority1 < priority2) {

      //   ass.dataType = op1.type;
      //   Result temp;
      //   temp.setValue(ass);

      //   func->function.add(Instruction(ctx, Instruction::Abstr::CONVERT, rhs, op1, ass));
      //   func->function.add(Instruction(ctx, getAbstr(), op1, temp, ass));
      // }
      // else
      // {
      //   op1.to(id.dataType);
      //   ass.dataType = rhs.id.dataType;
      //   table->AddSymbol(ass);
      //   func->function.add(Instruction(ctx, getAbstr(), op1, rhs, ass));
      // }
      res.setValue(*ass);
    }
    virtual void perform(Identifier& id1, Identifier& id2, Result& res) {

      if (!validType1(id1.dataType)) throw id1.dataType;
      if (!validType2(id2.dataType)) throw id2.dataType;
      
      int priority1 = fetchPriority(id1.dataType);
      int priority2 = fetchPriority(id2.dataType);

      Result lhs;
      lhs.setValue(id1);
      Result rhs;
      rhs.setValue(id2);

      // string tempname = "__temp_var_" + std::to_string(table->temp_vars++) + "__";
      // Identifier ass;
      // ass.name = tempname;
      Identifier* ass;

      if (priority1 == priority2)
      {
        // ass.dataType = id1.dataType;
        ass = &manageTemps(id1.dataType);
        func->function.add(Instruction(ctx, getAbstr(), getCond(), lhs, rhs, *ass));
      }
      else if (priority1 < priority2)
      {
        // ass.dataType = id1.dataType;
        ass = &manageTemps(id1.dataType);
        Result temp;
        temp.setValue(*ass);

        func->function.add(Instruction(ctx, Instruction::Abstr::CONVERT, rhs, lhs, *ass));
        func->function.add(Instruction(ctx, getAbstr(), getCond(), lhs, temp, *ass));
      }
      else if (priority1 > priority2)
      {
        // ass.dataType = id2.dataType;
        ass = &manageTemps(id2.dataType);
        Result temp;
        temp.setValue(*ass);

        func->function.add(Instruction(ctx, Instruction::Abstr::CONVERT, lhs, rhs, *ass));
        func->function.add(Instruction(ctx, getAbstr(), getCond(), temp, rhs, *ass));
      }
      res.setValue(*ass);
    }

    /////////////////////////////////////////
    // Unary expressions
    /////////////////////////////////////////

    virtual void perform(long double v1, Result& res) { throw long_double_; }
    virtual void perform(double v1, Result& res) { throw double_; }
    virtual void perform(float v1, Result& res) { throw float_; }
    virtual void perform(unsigned long long v1, Result& res) { throw unsigned_long_long_; }
    virtual void perform(long long v1, Result& res) { throw long_long_; }
    virtual void perform(unsigned long v1, Result& res) { throw unsigned_long_; }
    virtual void perform(long v1, Result& res) { throw long_; }
    virtual void perform(unsigned v1, Result& res) { throw unsigned_; }
    virtual void perform(int v1, Result& res) { throw int_; }
    virtual void perform(unsigned short v1, Result& res) { throw unsigned_short_; }
    virtual void perform(unsigned char v1,  Result& res) { throw unsigned_char_; }
    virtual void perform(signed char v1, Result& res) { throw signed_char_; }
    virtual void perform(char v1, Result& res) { throw char_; }

    virtual void perform(Identifier& id, Result& res) {

      if (!validType1(id.dataType)) throw id.dataType;

      Result lhs;
      lhs.setValue(id);

      // string tempname = "__temp_var_" + std::to_string(table->temp_vars++) + "__";
      // Identifier ass;
      // ass.name = tempname;

      // ass.dataType = lhs.id.dataType;
      Identifier& ass = manageTemps(lhs.id->dataType);
      func->function.add(Instruction(ctx, getAbstr(), lhs, ass));

      res.setValue(ass);
    }

    // for functions
    virtual void perform(Identifier& f, vector<Result> args, Result& res) { throw 1; }

    // needs access to the symbols table!
    SymbolTable* table;
    // will add the instruction to the current function
    Identifier* func;
    // Needs the parse tree node
    ParserRuleContext* ctx;
    // Also needs pointer to the compiler object for easy error reporting
    Compiler* comp;
};

#define STANDARD_PERFORM \
  using OperatorBase::OperatorBase; \
  void perform(long double v1, long double v2, Result& res) override { operation(v1, v2, res); } \
  void perform(double v1, double v2, Result& res) override { operation(v1, v2, res); } \
  void perform(float v1, float v2, Result& res) override { operation(v1, v2, res); } \
  void perform(unsigned long long v1, unsigned long long v2, Result& res) override { operation(v1, v2, res); } \
  void perform(long long v1, long long v2, Result& res) override { operation(v1, v2, res); } \
  void perform(unsigned long v1, unsigned long v2, Result& res) override { operation(v1, v2, res); } \
  void perform(long v1, long v2, Result& res) override { operation(v1, v2, res); } \
  void perform(unsigned v1, unsigned v2, Result& res) override { operation(v1, v2, res); } \
  void perform(int v1, int v2, Result& res) override { operation(v1, v2, res); } \
  void perform(unsigned short v1, unsigned short v2, Result& res) override { operation(v1, v2, res); } \
  void perform(unsigned char v1, unsigned char v2, Result& res) override { operation(v1, v2, res); } \
  void perform(signed char v1, signed char v2, Result& res) override { operation(v1, v2, res); } \
  void perform(char v1, char v2, Result& res) override { operation(v1, v2, res); }

#define INTEGRAL_PERFORM \
  using OperatorBase::OperatorBase; \
  void perform(unsigned long long v1, unsigned long long v2, Result& res) override { operation(v1, v2, res); } \
  void perform(long long v1, long long v2, Result& res) override { operation(v1, v2, res); } \
  void perform(unsigned long v1, unsigned long v2, Result& res) override { operation(v1, v2, res); } \
  void perform(long v1, long v2, Result& res) override { operation(v1, v2, res); } \
  void perform(unsigned v1, unsigned v2, Result& res) override { operation(v1, v2, res); } \
  void perform(int v1, int v2, Result& res) override { operation(v1, v2, res); } \
  void perform(unsigned short v1, unsigned short v2, Result& res) override { operation(v1, v2, res); } \
  void perform(unsigned char v1, unsigned char v2, Result& res) override { operation(v1, v2, res); } \
  void perform(signed char v1, signed char v2, Result& res) override { operation(v1, v2, res); } \
  void perform(char v1, char v2, Result& res) override { operation(v1, v2, res); }

#define OPERATION(operator) \
  template <typename T> \
  void operation(T v1, T v2, Result& res) \
  { \
    T out = v1 operator v2; \
    res.setValue(out); \
  }

#define OPERATION_LOG(operator) \
  template <typename T> \
  void operation(T v1, T v2, Result& res) \
  { \
    int out = v1 operator v2; \
    res.setValue(out); \
  }

#define INTEGRAL_CHECK \
  bool validType1(Type& t) override { \
    if (t == long_double_ || t == double_ || t == float_) \
      return false; \
    return true; \
  } \
  bool validType2(Type& t) override { \
    if (t == long_double_ || t == double_ || t == float_) \
      return false; \
    return true; \
  }

class Equal : public OperatorBase {

  public:

    STANDARD_PERFORM
    OPERATION_LOG(==)

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::CMP; }
    Instruction::Cond getCond() override { return Instruction::Cond::EQUAL; }
};

class NotEqual : public OperatorBase {

  public:

    STANDARD_PERFORM
    OPERATION_LOG(!=)

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::CMP; }
    Instruction::Cond getCond() override { return Instruction::Cond::NOT_EQUAL; }
};

class Greater : public OperatorBase {

  public:

    STANDARD_PERFORM
    OPERATION_LOG(>)

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::CMP; }
    Instruction::Cond getCond() override { return Instruction::Cond::GREATER; }
};

class Less : public OperatorBase {

  public:

    STANDARD_PERFORM
    OPERATION_LOG(<)

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::CMP; }
    Instruction::Cond getCond() override { return Instruction::Cond::LESS; }
};

class GreaterEqual : public OperatorBase {

  public:

    STANDARD_PERFORM
    OPERATION_LOG(>=)

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::CMP; }
    Instruction::Cond getCond() override { return Instruction::Cond::GREATER_EQUAL; }
};

class LessEqual : public OperatorBase {

  public:

    STANDARD_PERFORM
    OPERATION_LOG(<=)

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::CMP; }
    Instruction::Cond getCond() override { return Instruction::Cond::LESS_EQUAL; }
};

class Shiftl : public OperatorBase {

  public:

    INTEGRAL_PERFORM
    OPERATION(<<)
    INTEGRAL_CHECK

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::SHIFT_L; }
};

class Shiftr : public OperatorBase {

  public:

    INTEGRAL_PERFORM
    OPERATION(>>)
    INTEGRAL_CHECK

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::SHIFT_R; }
};

class BitAnd : public OperatorBase {

  public:

    INTEGRAL_PERFORM
    OPERATION(&)
    INTEGRAL_CHECK

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::BIT_AND; }
};

class BitOr : public OperatorBase {

  public:

    INTEGRAL_PERFORM
    OPERATION(|)
    INTEGRAL_CHECK

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::BIT_OR; }
};

class BitXor : public OperatorBase {

  public:

    INTEGRAL_PERFORM
    OPERATION(^)
    INTEGRAL_CHECK

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::BIT_XOR; }
};

// These actually shouldn't have any conversions!
class And : public OperatorBase {

  public:

    STANDARD_PERFORM
    OPERATION_LOG(&&)

    virtual void perform(Result& op1, Identifier& id, Result& res) override {
      Result rhs;
      rhs.setValue(id);

      // string tempname = "__temp_var_" + std::to_string(table->temp_vars++) + "__";
      // Identifier& ass;
      // ass.name = tempname;
      // ass.dataType.type_specifiers = {"int"};
      Type tt;
      tt.type_specifiers = {"int"};
      Identifier& ass = manageTemps(tt);

      func->function.add(Instruction(ctx, getAbstr(), op1, rhs, ass));
  
      res.setValue(ass);
    }

    virtual void perform(Identifier& id1, Result& op2, Result& res) override {
      Result lhs;
      lhs.setValue(id1);

      // string tempname = "__temp_var_" + std::to_string(table->temp_vars++) + "__";
      // Identifier& ass;
      // ass.name = tempname;
      // ass.dataType.type_specifiers = {"int"};
      Type tt;
      tt.type_specifiers = {"int"};
      Identifier& ass = manageTemps(tt);

      func->function.add(Instruction(ctx, getAbstr(), lhs, op2, ass));
  
      res.setValue(ass);
    }

    virtual void perform(Identifier& id1, Identifier& id2, Result& res) override {
      Result lhs;
      lhs.setValue(id1);
      Result rhs;
      rhs.setValue(id2);

      // string tempname = "__temp_var_" + std::to_string(table->temp_vars++) + "__";
      // Identifier& ass;
      // ass.name = tempname;
      // ass.dataType.type_specifiers = {"int"};
      Type tt;
      tt.type_specifiers = {"int"};
      Identifier& ass = manageTemps(tt);

      func->function.add(Instruction(ctx, getAbstr(), lhs, rhs, ass));
  
      res.setValue(ass);
    }

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::AND; }
};

class Or : public OperatorBase {

  public:

    STANDARD_PERFORM
    OPERATION_LOG(||)

    virtual void perform(Result& op1, Identifier& id, Result& res) override {
      Result rhs;
      rhs.setValue(id);

      // string tempname = "__temp_var_" + std::to_string(table->temp_vars++) + "__";
      // Identifier& ass;
      // ass.name = tempname;
      // ass.dataType.type_specifiers = {"int"};
      Type tt;
      tt.type_specifiers = {"int"};
      Identifier& ass = manageTemps(tt);

      func->function.add(Instruction(ctx, getAbstr(), op1, rhs, ass));
  
      res.setValue(ass);
    }

    virtual void perform(Identifier& id1, Result& op2, Result& res) override {
      Result lhs;
      lhs.setValue(id1);

      // string tempname = "__temp_var_" + std::to_string(table->temp_vars++) + "__";
      // Identifier& ass;
      // ass.name = tempname;
      // ass.dataType.type_specifiers = {"int"};
      Type tt;
      tt.type_specifiers = {"int"};
      Identifier& ass = manageTemps(tt);

      func->function.add(Instruction(ctx, getAbstr(), lhs, op2, ass));
  
      res.setValue(ass);
    }

    virtual void perform(Identifier& id1, Identifier& id2, Result& res) override {
      Result lhs;
      lhs.setValue(id1);
      Result rhs;
      rhs.setValue(id2);

      // string tempname = "__temp_var_" + std::to_string(table->temp_vars++) + "__";
      // Identifier& ass;
      // ass.name = tempname;
      // ass.dataType.type_specifiers = {"int"};
      Type tt;
      tt.type_specifiers = {"int"};
      Identifier& ass = manageTemps(tt);

      func->function.add(Instruction(ctx, getAbstr(), lhs, rhs, ass));
  
      res.setValue(ass);
    }

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::OR; }
};

class Mult : public OperatorBase {

  public:

    STANDARD_PERFORM
    OPERATION(*)

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::MULT; }
};

class Div : public OperatorBase {

  public:

    STANDARD_PERFORM
    OPERATION(/)

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::DIV; }
};

// TODO -- make this work with floats on lhs
class Mod : public OperatorBase {

  public:

    INTEGRAL_PERFORM
    OPERATION(%)

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::MOD; }
};

class Add : public OperatorBase {

  public:

    STANDARD_PERFORM
    OPERATION(+)

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::ADD; }
};

class Sub : public OperatorBase {

  public:

    STANDARD_PERFORM
    OPERATION(-)

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::SUB; }
};

class Negative : public OperatorBase {

  public:

    using OperatorBase::OperatorBase;

    void perform(long double v1, Result& res) override { operation(v1, res); }
    void perform(double v1, Result& res) override { operation(v1, res); }
    void perform(float v1, Result& res) override { operation(v1, res); }
    // void perform(unsigned long long v1, unsigned long long v2, Result& res) override { operation(v1, v2, res); }
    void perform(long long v1, Result& res) override { operation(v1, res); }
    // void perform(unsigned long v1, unsigned long v2, Result& res) override { operation(v1, v2, res); }
    void perform(long v1, Result& res) override { operation(v1, res); }
    // void perform(unsigned v1, unsigned v2, Result& res) override { operation(v1, v2, res); }
    void perform(int v1, Result& res) override { operation(v1, res); }
    // void perform(unsigned short v1, unsigned short v2, Result& res) override { operation(v1, v2, res); }
    // void perform(unsigned char v1, unsigned char v2, Result& res) override { operation(v1, v2, res); }
    void perform(signed char v1, signed char v2, Result& res) override { operation(v1, res); }
    // void perform(char v1, char v2, Result& res) override { operation(v1, v2, res); }

    template <typename T>
    void operation(T v1, Result& res)
    {
      T out = -v1;
      res.setValue(out);
    }

    bool validType1(Type& t) override { 
      if 
      (
        t == unsigned_long_long_ ||
        t == unsigned_long_ ||
        t == unsigned_ ||
        t == unsigned_short_ ||
        t == unsigned_char_
      )
      return false;

      return true;
    }

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::NEGATIVE; }
};

class Not : public OperatorBase {

  public:

    using OperatorBase::OperatorBase;

    // void perform(long double v1, Result& res) override { operation(v1, res); }
    // void perform(double v1, Result& res) override { operation(v1, res); }
    // void perform(float v1, Result& res) override { operation(v1, res); }
    void perform(unsigned long long v1, Result& res) override { operation(v1, res); }
    void perform(long long v1, Result& res) override { operation(v1, res); }
    void perform(unsigned long v1, Result& res) override { operation(v1, res); }
    void perform(long v1, Result& res) override { operation(v1, res); }
    void perform(unsigned v1, Result& res) override { operation(v1, res); }
    void perform(int v1, Result& res) override { operation(v1, res); }
    void perform(unsigned short v1, Result& res) override { operation(v1, res); }
    void perform(unsigned char v1, Result& res) override { operation(v1, res); }
    void perform(signed char v1, Result& res) override { operation(v1, res); }
    void perform(char v1, Result& res) override { operation(v1, res); }

    template <typename T>
    void operation(T v1, Result& res)
    {
      T out = ~v1;
      res.setValue(out);
    }

    bool validType1(Type& t) override { 
      if 
      (
        t == float_ ||
        t == double_ ||
        t == long_double_
      )
      return false;

      return true;
    }

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::NOT; }
};

class Negate : public OperatorBase {

  public:

    using OperatorBase::OperatorBase;

    void perform(long double v1, Result& res) override { operation(v1, res); }
    void perform(double v1, Result& res) override { operation(v1, res); }
    void perform(float v1, Result& res) override { operation(v1, res); }
    void perform(unsigned long long v1, Result& res) override { operation(v1, res); }
    void perform(long long v1, Result& res) override { operation(v1, res); }
    void perform(unsigned long v1, Result& res) override { operation(v1, res); }
    void perform(long v1, Result& res) override { operation(v1, res); }
    void perform(unsigned v1, Result& res) override { operation(v1, res); }
    void perform(int v1, Result& res) override { operation(v1, res); }
    void perform(unsigned short v1, Result& res) override { operation(v1, res); }
    void perform(unsigned char v1, Result& res) override { operation(v1, res); }
    void perform(signed char v1, Result& res) override { operation(v1, res); }
    void perform(char v1, Result& res) override { operation(v1, res); }

    template <typename T>
    void operation(T v1, Result& res)
    {
      int out = !v1;
      res.setValue(out);
    }

    virtual void perform(Identifier& id1, Result& res) override {
      Result lhs;
      lhs.setValue(id1);

      // string tempname = "__temp_var_" + std::to_string(table->temp_vars++) + "__";
      // Identifier& ass;
      // ass.name = tempname;
      // ass.dataType.type_specifiers = {"int"};
      Type tt;
      tt.type_specifiers = {"int"};
      Identifier& ass = manageTemps(tt);

      func->function.add(Instruction(ctx, getAbstr(), lhs, ass));
  
      res.setValue(ass);
    }

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::NEGATE; }
};

class Assign : public OperatorBase {

  public:

    using OperatorBase::OperatorBase;

    void perform(Identifier& id, Result& op2, Result& res) override {
      if (op2.type != id.dataType) {
        op2.to(id.dataType);
      }
      // in some cases this could be optimized out, but for now we'll
      // assume it should be an operation
      // id.assignment = op2;
      func->function.add(Instruction(ctx, getAbstr(), op2, id));
      
      // what is this supposed to be set to??
      res.setValue(0);
    }

    void perform(Identifier& id1, Identifier& id2, Result& res) override {
      if (id2.dataType != id1.dataType) 
      {
        Result temp;
        temp.setValue(id2);
        Result rhs;
        rhs.setValue(id1);
        func->function.add(Instruction(ctx, Instruction::Abstr::CONVERT, temp, rhs, id1));
      }
      else 
      {
        Result temp;
        temp.setValue(id2);
        func->function.add(Instruction(ctx, getAbstr(), temp, id1));
      }
      
      // what is this supposed to be set to??
      res.setValue(0);
    }

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::ASSIGN; }
};

class IncrPost : public OperatorBase {

  public:

    using OperatorBase::OperatorBase;

    void perform(Identifier& id, Result& res) override {
      
      Result op2;
      op2.setValue(1);
      op2.to(id.dataType);

      Result lhs;
      lhs.setValue(id);

      // If any post-expr operation happens on the same identifier,
      // then we have a warning
      for (auto inst : table->postExpr)
      {
        if (*inst.assignment == id) {
          string warnmess = "ambiguous postfix operation order";
          // TODO -- add error option with mutliple nodes so we
          // can avoid referencing the same node twice with this (like if there's three!)
          comp->addNodeWarning(ctx, warnmess);
          comp->addNodeWarning(inst.ctx, warnmess);
        }
      }
      
      table->postExpr.push_back(Instruction(ctx, getAbstr(), lhs, op2, id));
      
      res.setValue(id);
    }

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::ADD; }
};

class DecrPost : public OperatorBase {

  public:

    using OperatorBase::OperatorBase;

    void perform(Identifier& id, Result& res) override {
      
      Result op2;
      op2.setValue(1);
      op2.to(id.dataType);

      Result lhs;
      lhs.setValue(id);

      // If any post-expr operation happens on the same identifier,
      // then we have a warning
      for (auto inst : table->postExpr)
      {
        if (*inst.assignment == id) {
          string warnmess = "ambiguous postfix operation order";
          // TODO -- add error option with mutliple nodes so we
          // can avoid referencing the same node twice with this (like if there's three!)
          comp->addNodeWarning(ctx, warnmess);
          comp->addNodeWarning(inst.ctx, warnmess);
        }
      }
      
      table->postExpr.push_back(Instruction(ctx, getAbstr(), lhs, op2, id));
      
      res.setValue(id);
    }

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::SUB; }
};

class IncrPre : public OperatorBase {

  public:

    using OperatorBase::OperatorBase;

    void perform(Identifier& id, Result& res) override {
      
      Result op2;
      op2.setValue(1);
      op2.to(id.dataType);

      Result lhs;
      lhs.setValue(id);

      // TODO -- come up with way to warn about multiple prefix operations
      // for (auto inst : table->postExpr)
      // {
      //   if (inst.assignment == id) {
      //     string warnmess = "ambiguous postfix operation order";
      //     // TODO -- add error option with mutliple nodes so we
      //     // can avoid referencing the same node twice with this (like if there's three!)
      //     comp->addRuleErr(ctx, warnmess);
      //     comp->addRuleErr(inst.ctx, warnmess);
      //   }
      // }
      
      func->function.add(Instruction(ctx, getAbstr(), lhs, op2, id));
      
      res.setValue(id);
    }

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::ADD; }
};

class DecrPre : public OperatorBase {

  public:

    using OperatorBase::OperatorBase;

    void perform(Identifier& id, Result& res) override {
      
      Result op2;
      op2.setValue(1);
      op2.to(id.dataType);

      Result lhs;
      lhs.setValue(id);

      // TODO -- come up with way to warn about multiple prefix operations
      // for (auto inst : table->postExpr)
      // {
      //   if (inst.assignment == id) {
      //     string warnmess = "ambiguous postfix operation order";
      //     // TODO -- add error option with mutliple nodes so we
      //     // can avoid referencing the same node twice with this (like if there's three!)
      //     comp->addRuleErr(ctx, warnmess);
      //     comp->addRuleErr(inst.ctx, warnmess);
      //   }
      // }
      
      func->function.add(Instruction(ctx, getAbstr(), lhs, op2, id));
      
      res.setValue(id);
    }

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::SUB; }
};

// class Convert {
//   public:
//     STANDARD_PERFORM

//     template <typename T>
//     void operation(T v1, T v2, Result& res)
//     {
//       int out = !v1;
//       res.setValue(out);
//     }

//     Instruction::Abstr getAbstr() override { return Instruction::Abstr::CONVERT; }

// };

class Call : public OperatorBase {
  public:
    using OperatorBase::OperatorBase;

    void perform(Identifier& f, vector<Result> args, Result& res) override
    {
      Identifier& ass = manageTemps(f.dataType);
      
      for (int i = args.size() - 1; i > -1; i--) {
        if (args[i].getType() != f.members[i].dataType) {
          Identifier& tempass = manageTemps(f.members[i].dataType);
          if (args[i].isConst())
          {
            Result res = args[i];
            res.to(f.members[i].dataType);
            func->function.add(Instruction(ctx, Instruction::Abstr::ASSIGN, res, tempass));
          }
          else
          {
            func->function.add(Instruction(ctx, Instruction::Abstr::CONVERT, args[i], tempass));
          }
          args[i].setValue(tempass);
        }
      }
      
      func->function.add(Instruction(ctx, getAbstr(), f, args, ass));
  
      res.setValue(ass);
    }

    Instruction::Abstr getAbstr() override { return Instruction::Abstr::CALL; }
};