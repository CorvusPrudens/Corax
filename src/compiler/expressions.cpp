#include <iostream>
#include <string>
#include <stdio.h>

#include "compiler.h"
#include "operators.h"

using antlrcpp::Any;
using std::string;

////////////////////////////////////////////////
// EXPRESSIONS
////////////////////////////////////////////////

void Compiler::operation(antlr4::tree::ParseTree* ctx, Result op1, Result op2, OperatorBase& oper)
{
  Result res;

  if (op1.isConst() && op2.isConst()) {
    int priority1 = fetchPriority(op1.type);
    int priority2 = fetchPriority(op2.type);
    if (priority1 < priority2)
    {
      op2.to(op1.type);
    }
    else if (priority2 < priority1)
    {
      op1.to(op2.type);
    }

    Type type = op1.type;

    if (type == long_double_) {
      auto v1 = op1.as<long double>();
      auto v2 = op2.as<long double>();
      oper.perform(v1, v2, res);
    } else if (type == double_) {
      auto v1 = op1.as<double>();
      auto v2 = op2.as<double>();
      oper.perform(v1, v2, res);
    } else if (type == float_) {
      auto v1 = op1.as<float>();
      auto v2 = op2.as<float>();
      oper.perform(v1, v2, res);
    } else if (type == unsigned_long_long_) {
      auto v1 = op1.as<unsigned long long>();
      auto v2 = op2.as<unsigned long long>();
      oper.perform(v1, v2, res);
    } else if (type == long_long_) {
      auto v1 = op1.as<long long>();
      auto v2 = op2.as<long long>();
      oper.perform(v1, v2, res);
    } else if (type == unsigned_long_) {
      auto v1 = op1.as<unsigned long>();
      auto v2 = op2.as<unsigned long>();
      oper.perform(v1, v2, res);
    } else if (type == long_) {
      auto v1 = op1.as<long>();
      auto v2 = op2.as<long>();
      oper.perform(v1, v2, res);
    } else if (type == unsigned_) {
      auto v1 = op1.as<unsigned>();
      auto v2 = op2.as<unsigned>();
      oper.perform(v1, v2, res);
    } else if (type == int_) {
      auto v1 = op1.as<int>();
      auto v2 = op2.as<int>();
      oper.perform(v1, v2, res);
    } else if (type == unsigned_short_) {
      auto v1 = op1.as<unsigned short>();
      auto v2 = op2.as<unsigned short>();
      oper.perform(v1, v2, res);
    } else if (type == short_) {
      auto v1 = op1.as<short>();
      auto v2 = op2.as<short>();
      oper.perform(v1, v2, res);
    } else if (type == unsigned_char_) {
      auto v1 = op1.as<unsigned char>();
      auto v2 = op2.as<unsigned char>();
      oper.perform(v1, v2, res);
    } else if (type == signed_char_) {
      auto v1 = op1.as<signed char>();
      auto v2 = op2.as<signed char>();
      oper.perform(v1, v2, res);
    } else if (type == char_) {
      auto v1 = op1.as<char>();
      auto v2 = op2.as<char>();
      oper.perform(v1, v2, res);
    } else if (type == void_) {
      throw 1;
    } else {
      throw 2;
    }
  }
  else if (op1.isConst()) {
    oper.perform(op1, *op2.id, res);
  } else if (op2.isConst()) {
    oper.perform(*op1.id, op2, res);
  } else {
    oper.perform(*op1.id, *op2.id, res);
  }

  results.put(ctx, res);
}

void Compiler::operation(antlr4::tree::ParseTree* ctx, Result op1, OperatorBase& oper)
{
  Result res;

  if (op1.isConst()) {

    Type type = op1.type;

    if (type == long_double_) {
      auto v1 = op1.as<long double>();
      oper.perform(v1, res);
    } else if (type == double_) {
      auto v1 = op1.as<double>();
      oper.perform(v1, res);
    } else if (type == float_) {
      auto v1 = op1.as<float>();
      oper.perform(v1, res);
    } else if (type == unsigned_long_long_) {
      auto v1 = op1.as<unsigned long long>();
      oper.perform(v1, res);
    } else if (type == long_long_) {
      auto v1 = op1.as<long long>();
      oper.perform(v1, res);
    } else if (type == unsigned_long_) {
      auto v1 = op1.as<unsigned long>();
      oper.perform(v1, res);
    } else if (type == long_) {
      auto v1 = op1.as<long>();
      oper.perform(v1, res);
    } else if (type == unsigned_) {
      auto v1 = op1.as<unsigned>();
      oper.perform(v1, res);
    } else if (type == int_) {
      auto v1 = op1.as<int>();
      oper.perform(v1, res);
    } else if (type == unsigned_short_) {
      auto v1 = op1.as<unsigned short>();
      oper.perform(v1, res);
    } else if (type == short_) {
      auto v1 = op1.as<short>();
      oper.perform(v1, res);
    } else if (type == unsigned_char_) {
      auto v1 = op1.as<unsigned char>();
      oper.perform(v1, res);
    } else if (type == signed_char_) {
      auto v1 = op1.as<signed char>();
      oper.perform(v1, res);
    } else if (type == char_) {
      auto v1 = op1.as<char>();
      oper.perform(v1, res);
    } else if (type == void_) {
      throw 1;
    } else {
      throw 2;
    }
  } else {
    oper.perform(*op1.id, res);
  }

  results.put(ctx, res);
}

Any Compiler::visitCall(CoraxParser::CallContext* ctx)
{
  
  visitChildren(ctx);
  Call oper(ctx, currentScope, currentFunction, this);
  vector<Result> args;
  if (ctx->arg_expr_list() != nullptr)
  {
    for (auto* ctxs : ctx->arg_expr_list()->expr_assi())
    {
      args.push_back(results.get(ctxs));
    }
  }

  // TODO -- needs help with indirect calls
  // TODO -- need way to detect if what's called is actually a function (or function pointer)
  Result res;
  try {
    Identifier& f = currentScope->GetSymbol(ctx->expr_primary()->getText());
    if (f.members.size() != args.size()) {
      // TODO -- this could be made more specific
      addNodeError(ctx, "argument count doesn't match function definition");
      res.setValue(0);
    } else {
      oper.perform(f, args, res);
      if (graphing) graph.Addc(ctx->expr_primary()->getText());
    }
  } catch (int e) {
    addNodeError(ctx, "attempting to call undefined function");
    res.setValue(0);
  }
  results.put(ctx, res);
  return nullptr;
}

Any Compiler::visitStatReturn(CoraxParser::StatReturnContext* ctx)
{
  visitChildren(ctx);
  if (ctx->expression() != nullptr)
  {
    Instruction inst_prep(ctx, Instruction::RETURN_PREP, results.get(ctx->expression()));
    inst_prep.scope = currentScope;
    currentFunction->function.add(inst_prep);

    Instruction inst(ctx, Instruction::RETURN, results.get(ctx->expression()));
    inst.scope = currentScope;
    currentFunction->function.add(inst);
  }
  else
  {
    Result res;
    res.setValue(0);
    res.type.type_specifiers = {"void"};
    
    Instruction inst_prep(ctx, Instruction::RETURN_PREP, res);
    inst_prep.scope = currentScope;
    currentFunction->function.add(inst_prep);

    Instruction inst(ctx, Instruction::RETURN, res);
    inst.scope = currentScope;
    currentFunction->function.add(inst);
  }
  return nullptr;
}

Any Compiler::visitStatLabeled(CoraxParser::StatLabeledContext* ctx)
{
  Identifier id;
  id.type = Identifier::LABEL;
  id.name = ctx->IDENTIFIER()->getText();
  try {
    currentScope->AddSymbol(id);
    currentFunction->function.add(Instruction(ctx, Instruction::LABEL, currentScope->GetLast()));
  } catch (int e) {
    string errmess = "label name overrides local variable";
    addNodeError(ctx, errmess);
  }
  return visitChildren(ctx);
}

// TODO -- the architecture should update flags IN ADDITION TO return 1 / 0 results for comparisons!
Any Compiler::visitStatIf(CoraxParser::StatIfContext* ctx)
{
  PushIf();

  Identifier begin;
  begin.type = Identifier::LABEL;
  begin.name = LabelIf();
  currentScope->AddSymbol(begin);
  Identifier& bptr = currentScope->GetLast();

  Identifier end;
  end.type = Identifier::LABEL;
  end.name = LabelIf(false);
  currentScope->AddSymbol(end);
  Identifier& eptr = currentScope->GetLast();

  visit(ctx->expression());
  Result check = results.get(ctx->expression());
  Result constant;
  constant.setValue(0);
  Instruction inst(ctx, Instruction::CONDITIONAL, Instruction::GREATER, check, constant, bptr, eptr);
  currentFunction->function.add(inst);

  currentFunction->function.add(Instruction(ctx, Instruction::LABEL, bptr));
  visit(ctx->statement());
  currentFunction->function.add(Instruction(ctx, Instruction::LABEL, eptr));

  PopIf();

  return nullptr;
}
Any Compiler::visitStatIfElse(CoraxParser::StatIfElseContext* ctx)
{
  return visitChildren(ctx);
}

// Simple passthroughs
Any Compiler::visitExprCast(CoraxParser::ExprCastContext* ctx)
{
  visitChildren(ctx);
  Result res = results.get(ctx->children[0]);
  results.put(ctx, res);
  return nullptr;
}

Any Compiler::visitExprMulti(CoraxParser::ExprMultiContext* ctx)
{
  visitChildren(ctx);
  Result res = results.get(ctx->children[0]);
  results.put(ctx, res);
  return nullptr;
}

Any Compiler::visitExprTern(CoraxParser::ExprTernContext* ctx)
{
  visitChildren(ctx);
  Result res = results.get(ctx->children[0]);
  results.put(ctx, res);
  return nullptr;
}

Any Compiler::visitExprAssi(CoraxParser::ExprAssiContext* ctx)
{
  visitChildren(ctx);
  Result res = results.get(ctx->children[0]);
  results.put(ctx, res);
  return nullptr;
}

Any Compiler::visitArg_expr_list(CoraxParser::Arg_expr_listContext* ctx)
{
  visitChildren(ctx);
  // // Just in case we 
  // Result res;
  // res.setValue(0);
  // results.put(ctx, res);
  return nullptr;
}

Any Compiler::visitExprExpression(CoraxParser::ExprExpressionContext* ctx)
{
  visitChildren(ctx);
  Result res = results.get(ctx->children[0]);
  results.put(ctx, res);
  return nullptr;
}

// Simple primaries
Any Compiler::visitIdentifier(CoraxParser::IdentifierContext* ctx)
{
  Result res;
  try {
    Identifier& id = currentScope->GetSymbol(ctx->getText());
    res.setValue(id);
  } catch (int e) {
    string errmess = "undefined identifier \"" + ctx->getText() + "\"";
    addNodeError(ctx, errmess);
  }
  results.put(ctx, res);
  return nullptr;
}

Any Compiler::visitConstInt(CoraxParser::ConstIntContext* ctx)
{
  int value = stoi(ctx->getText());
  Result res;
  res.setValue(value);
  results.put(ctx, res);
  return nullptr;
}

Any Compiler::visitConstFlt(CoraxParser::ConstFltContext* ctx)
{
  float value = stof(ctx->getText());
  Result res;
  res.setValue(value);
  results.put(ctx, res);
  return nullptr;
}

// not sure what to do with a string yet
Any Compiler::visitString(CoraxParser::StringContext* ctx)
{
  Result res;
  results.put(ctx, res);
  return nullptr;
}

Any Compiler::visitGroup(CoraxParser::GroupContext* ctx)
{
  visitChildren(ctx);
  Result res = results.get(ctx->expression());
  results.put(ctx, res);
  return nullptr;
}

// simple arithmetic
Any Compiler::visitMult(CoraxParser::MultContext* ctx)
{
  visitChildren(ctx);
  Mult oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_arith()[0]), results.get(ctx->expr_arith()[1]), oper);

  return nullptr;
}

// Assignments
Any Compiler::visitAssignment(CoraxParser::AssignmentContext* ctx)
{
  visitChildren(ctx);
  Assign oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_primary()), results.get(ctx->expr_assi()), oper);

  return nullptr;
}

Any Compiler::visitDereference(CoraxParser::DereferenceContext *ctx) 
{
  return visitChildren(ctx);
}

Any Compiler::visitNegation(CoraxParser::NegationContext *ctx) 
{
  visitChildren(ctx);
  Negate oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_cast()), oper);

  return nullptr;
}

Any Compiler::visitIncrementUnary(CoraxParser::IncrementUnaryContext *ctx) 
{
  visitChildren(ctx);
  IncrPre oper(ctx, currentScope, currentFunction, this);
  try {
    operation(ctx, results.get(ctx->expr_primary()), oper);
  } catch (TypeDescriptor e) {
    string errmess = "unary increment requires modifiable lvalue";
    addNodeError(ctx, errmess);

    results.put(ctx, results.get(ctx->expr_primary()));
  }

  return nullptr;
}

Any Compiler::visitAddress(CoraxParser::AddressContext *ctx) 
{
  return visitChildren(ctx);
}

Any Compiler::visitIndexing(CoraxParser::IndexingContext *ctx) 
{
  return visitChildren(ctx);
}

Any Compiler::visitIncrementPost(CoraxParser::IncrementPostContext *ctx) 
{
  visitChildren(ctx);
  IncrPost oper(ctx, currentScope, currentFunction, this);
  try {
    operation(ctx, results.get(ctx->expr_primary()), oper);
  } catch (TypeDescriptor e) {
    string errmess = "postfix increment requires modifiable lvalue";
    addNodeError(ctx, errmess);

    results.put(ctx, results.get(ctx->expr_primary()));
  }

  return nullptr;
}

Any Compiler::visitSizeof(CoraxParser::SizeofContext *ctx) 
{
  return visitChildren(ctx);
}

Any Compiler::visitPositive(CoraxParser::PositiveContext *ctx) 
{
  visitChildren(ctx);
  results.put(ctx, results.get(ctx->expr_cast()));
  return nullptr;
}

Any Compiler::visitDecrementPost(CoraxParser::DecrementPostContext *ctx) 
{
  visitChildren(ctx);
  DecrPost oper(ctx, currentScope, currentFunction, this);
  try {
    operation(ctx, results.get(ctx->expr_primary()), oper);
  } catch (TypeDescriptor e) {
    string errmess = "postfix decrement requires modifiable lvalue";
    addNodeError(ctx, errmess);

    results.put(ctx, results.get(ctx->expr_primary()));
  }

  return nullptr;
}

Any Compiler::visitDecrementUnary(CoraxParser::DecrementUnaryContext *ctx) 
{
  visitChildren(ctx);
  DecrPre oper(ctx, currentScope, currentFunction, this);
  try {
    operation(ctx, results.get(ctx->expr_primary()), oper);
  } catch (TypeDescriptor e) {
    string errmess = "unary decrement requires modifiable lvalue";
    addNodeError(ctx, errmess);

    results.put(ctx, results.get(ctx->expr_primary()));
  }

  return nullptr;
}

Any Compiler::visitSizeofType(CoraxParser::SizeofTypeContext *ctx) 
{
  return visitChildren(ctx);
}

Any Compiler::visitIndirectMember(CoraxParser::IndirectMemberContext *ctx) 
{
  return visitChildren(ctx);
}

Any Compiler::visitNegative(CoraxParser::NegativeContext *ctx) 
{
  visitChildren(ctx);
  Negative oper(ctx, currentScope, currentFunction, this);
  try {
    operation(ctx, results.get(ctx->expr_cast()), oper);
  } catch (TypeDescriptor e) {
    string errmess = "behavior undefined for unsigned type \"" + e.to_string();
    addNodeError(ctx, errmess + "\"");

    results.put(ctx, results.get(ctx->expr_cast()));
  } catch (Type e) {
    string errmess = "behavior undefined for unsigned type \"" + e.to_string();
    addNodeError(ctx, errmess + "\"");

    results.put(ctx, results.get(ctx->expr_cast()));
  }

  return nullptr;
}

Any Compiler::visitNot(CoraxParser::NotContext *ctx) 
{
  visitChildren(ctx);
  Not oper(ctx, currentScope, currentFunction, this);
  try {
    operation(ctx, results.get(ctx->expr_cast()), oper);
  } catch (TypeDescriptor e) {
    string errmess = "behavior undefined for type \"" + e.to_string();
    addNodeError(ctx, errmess + "\"");

    results.put(ctx, results.get(ctx->expr_cast()));
  } catch (Type e) {
    string errmess = "behavior undefined for type \"" + e.to_string();
    addNodeError(ctx, errmess + "\"");

    results.put(ctx, results.get(ctx->expr_cast()));
  }

  return nullptr;
}

Any Compiler::visitMember(CoraxParser::MemberContext *ctx) 
{
  return visitChildren(ctx);
}

Any Compiler::visitCast(CoraxParser::CastContext *ctx) 
{
  return visitChildren(ctx);
}

Any Compiler::visitMinus(CoraxParser::MinusContext *ctx) 
{
  visitChildren(ctx);
  Sub oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_arith()[0]), results.get(ctx->expr_arith()[1]), oper);

  return nullptr;
}

Any Compiler::visitMod(CoraxParser::ModContext *ctx) 
{
  visitChildren(ctx);
  Mod oper(ctx, currentScope, currentFunction, this);
  try {
    operation(ctx, results.get(ctx->expr_arith()[0]), results.get(ctx->expr_arith()[1]), oper);
  } catch (TypeDescriptor e) {
    string errmess = "modulo operator cannot accept operands of type \"" + e.to_string();
    addNodeError(ctx, errmess + "\"");
    // we'll just put a const 0 for now in case of error
    Result res;
    res.setValue(0);
    results.put(ctx, res);
  }
  
  return nullptr;
}

Any Compiler::visitOr(CoraxParser::OrContext *ctx) 
{
  visitChildren(ctx);
  Or oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_arith()[0]), results.get(ctx->expr_arith()[1]), oper);

  return nullptr;
}

Any Compiler::visitNotEqual(CoraxParser::NotEqualContext *ctx) 
{
  visitChildren(ctx);
  NotEqual oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_arith()[0]), results.get(ctx->expr_arith()[1]), oper);

  return nullptr;
}

Any Compiler::visitLess(CoraxParser::LessContext *ctx) 
{
  visitChildren(ctx);
  Less oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_arith()[0]), results.get(ctx->expr_arith()[1]), oper);

  return nullptr;
}

Any Compiler::visitBit_or(CoraxParser::Bit_orContext *ctx) 
{
  visitChildren(ctx);
  BitOr oper(ctx, currentScope, currentFunction, this);
  try {
    operation(ctx, results.get(ctx->expr_arith()[0]), results.get(ctx->expr_arith()[1]), oper);
  } catch (TypeDescriptor e) {
    string errmess = "operation undefined for operand of type \"" + e.to_string();
    addNodeError(ctx, errmess + "\"");
    // we'll just put a const 0 for now in case of error
    Result res;
    res.setValue(0);
    results.put(ctx, res);
  } catch (Type e) {
    string errmess = "operation undefined for operand of type \"" + e.to_string();
    addNodeError(ctx, errmess + "\"");
    // we'll just put a const 0 for now in case of error
    Result res;
    res.setValue(0);
    results.put(ctx, res);
  }
  
  return nullptr;
}

Any Compiler::visitPlus(CoraxParser::PlusContext *ctx) 
{
  visitChildren(ctx);
  Add oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_arith()[0]), results.get(ctx->expr_arith()[1]), oper);

  return nullptr;
}

Any Compiler::visitGreater_equal(CoraxParser::Greater_equalContext *ctx) 
{
  visitChildren(ctx);
  GreaterEqual oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_arith()[0]), results.get(ctx->expr_arith()[1]), oper);

  return nullptr;
}

Any Compiler::visitDiv(CoraxParser::DivContext *ctx) 
{
  visitChildren(ctx);
  Div oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_arith()[0]), results.get(ctx->expr_arith()[1]), oper);

  return nullptr;
}

Any Compiler::visitEqual(CoraxParser::EqualContext *ctx) 
{
  visitChildren(ctx);
  Equal oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_arith()[0]), results.get(ctx->expr_arith()[1]), oper);

  return nullptr;
}

Any Compiler::visitShiftLeft(CoraxParser::ShiftLeftContext *ctx) 
{
  visitChildren(ctx);
  Shiftl oper(ctx, currentScope, currentFunction, this);
  try {
    operation(ctx, results.get(ctx->expr_arith()[0]), results.get(ctx->expr_arith()[1]), oper);
  } catch (TypeDescriptor e) {
    string errmess = "operation undefined for operand of type \"" + e.to_string();
    addNodeError(ctx, errmess + "\"");
    // we'll just put a const 0 for now in case of error
    Result res;
    res.setValue(0);
    results.put(ctx, res);
  } catch (Type e) {
    string errmess = "operation undefined for operand of type \"" + e.to_string();
    addNodeError(ctx, errmess + "\"");
    // we'll just put a const 0 for now in case of error
    Result res;
    res.setValue(0);
    results.put(ctx, res);
  }
  
  return nullptr;
}

Any Compiler::visitShiftRight(CoraxParser::ShiftRightContext *ctx) 
{
  visitChildren(ctx);
  Shiftr oper(ctx, currentScope, currentFunction, this);
  try {
    operation(ctx, results.get(ctx->expr_arith()[0]), results.get(ctx->expr_arith()[1]), oper);
  } catch (TypeDescriptor e) {
    string errmess = "operation undefined for operand of type \"" + e.to_string();
    addNodeError(ctx, errmess + "\"");
    // we'll just put a const 0 for now in case of error
    Result res;
    res.setValue(0);
    results.put(ctx, res);
  } catch (Type e) {
    string errmess = "operation undefined for operand of type \"" + e.to_string();
    addNodeError(ctx, errmess + "\"");
    // we'll just put a const 0 for now in case of error
    Result res;
    res.setValue(0);
    results.put(ctx, res);
  }
  
  return nullptr;
}

Any Compiler::visitBit_xor(CoraxParser::Bit_xorContext *ctx) 
{
  visitChildren(ctx);
  BitXor oper(ctx, currentScope, currentFunction, this);
  try {
    operation(ctx, results.get(ctx->expr_arith()[0]), results.get(ctx->expr_arith()[1]), oper);
  } catch (TypeDescriptor e) {
    string errmess = "operation undefined for operand of type \"" + e.to_string();
    addNodeError(ctx, errmess + "\"");
    // we'll just put a const 0 for now in case of error
    Result res;
    res.setValue(0);
    results.put(ctx, res);
  } catch (Type e) {
    string errmess = "operation undefined for operand of type \"" + e.to_string();
    addNodeError(ctx, errmess + "\"");
    // we'll just put a const 0 for now in case of error
    Result res;
    res.setValue(0);
    results.put(ctx, res);
  }
  
  return nullptr;
}

Any Compiler::visitAnd(CoraxParser::AndContext *ctx) 
{
  visitChildren(ctx);
  And oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_arith()[0]), results.get(ctx->expr_arith()[1]), oper);

  return nullptr;
}

Any Compiler::visitBit_and(CoraxParser::Bit_andContext *ctx) 
{
  visitChildren(ctx);
  BitAnd oper(ctx, currentScope, currentFunction, this);
  try {
    operation(ctx, results.get(ctx->expr_arith()[0]), results.get(ctx->expr_arith()[1]), oper);
  } catch (TypeDescriptor e) {
    string errmess = "operation undefined for operand of type \"" + e.to_string();
    addNodeError(ctx, errmess + "\"");
    // we'll just put a const 0 for now in case of error
    Result res;
    res.setValue(0);
    results.put(ctx, res);
  } catch (Type e) {
    string errmess = "operation undefined for operand of type \"" + e.to_string();
    addNodeError(ctx, errmess + "\"");
    // we'll just put a const 0 for now in case of error
    Result res;
    res.setValue(0);
    results.put(ctx, res);
  }
  
  return nullptr;
}

Any Compiler::visitGreater(CoraxParser::GreaterContext *ctx) 
{
  visitChildren(ctx);
  Greater oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_arith()[0]), results.get(ctx->expr_arith()[1]), oper);

  return nullptr;
}

Any Compiler::visitLess_equal(CoraxParser::Less_equalContext *ctx) 
{
  visitChildren(ctx);
  LessEqual oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_arith()[0]), results.get(ctx->expr_arith()[1]), oper);

  return nullptr;
}

Any Compiler::visitTernary(CoraxParser::TernaryContext *ctx) 
{
  return visitChildren(ctx);
}

Any Compiler::visitAssignmentMult(CoraxParser::AssignmentMultContext *ctx) 
{
  visitChildren(ctx);
  Mult oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_primary()), results.get(ctx->expr_assi()), oper);
  Assign oper2(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_primary()), results.get(ctx), oper2);

  return nullptr;
}

Any Compiler::visitAssignmentDiv(CoraxParser::AssignmentDivContext *ctx) 
{
  visitChildren(ctx);
  Div oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_primary()), results.get(ctx->expr_assi()), oper);
  Assign oper2(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_primary()), results.get(ctx), oper2);

  return nullptr;
}

Any Compiler::visitAssignmentMod(CoraxParser::AssignmentModContext *ctx) 
{
  visitChildren(ctx);
  Mod oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_primary()), results.get(ctx->expr_assi()), oper);
  Assign oper2(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_primary()), results.get(ctx), oper2);

  return nullptr;
}

Any Compiler::visitAssignmentPlus(CoraxParser::AssignmentPlusContext *ctx) 
{
  visitChildren(ctx);
  Add oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_primary()), results.get(ctx->expr_assi()), oper);
  Assign oper2(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_primary()), results.get(ctx), oper2);

  return nullptr;
}

Any Compiler::visitAssignmentMinus(CoraxParser::AssignmentMinusContext *ctx) 
{
  visitChildren(ctx);
  Sub oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_primary()), results.get(ctx->expr_assi()), oper);
  Assign oper2(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_primary()), results.get(ctx), oper2);

  return nullptr;
}

Any Compiler::visitAssignmentShiftLeft(CoraxParser::AssignmentShiftLeftContext *ctx) 
{
  visitChildren(ctx);
  Shiftl oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_primary()), results.get(ctx->expr_assi()), oper);
  Assign oper2(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_primary()), results.get(ctx), oper2);

  return nullptr;
}

Any Compiler::visitAssignmentShiftRight(CoraxParser::AssignmentShiftRightContext *ctx) 
{
  visitChildren(ctx);
  Shiftr oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_primary()), results.get(ctx->expr_assi()), oper);
  Assign oper2(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_primary()), results.get(ctx), oper2);

  return nullptr;
}

Any Compiler::visitAssignmentBitAnd(CoraxParser::AssignmentBitAndContext *ctx) 
{
  visitChildren(ctx);
  BitAnd oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_primary()), results.get(ctx->expr_assi()), oper);
  Assign oper2(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_primary()), results.get(ctx), oper2);

  return nullptr;
}

Any Compiler::visitAssignmentBitXor(CoraxParser::AssignmentBitXorContext *ctx) 
{
  visitChildren(ctx);
  BitXor oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_primary()), results.get(ctx->expr_assi()), oper);
  Assign oper2(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_primary()), results.get(ctx), oper2);

  return nullptr;
}

Any Compiler::visitAssignmentBitOr(CoraxParser::AssignmentBitOrContext *ctx) 
{
  visitChildren(ctx);
  BitOr oper(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_primary()), results.get(ctx->expr_assi()), oper);
  Assign oper2(ctx, currentScope, currentFunction, this);
  operation(ctx, results.get(ctx->expr_primary()), results.get(ctx), oper2);

  return nullptr;
}

Any Compiler::visitComma(CoraxParser::CommaContext *ctx) 
{
  return visitChildren(ctx);
}

Any Compiler::visitExpr_const(CoraxParser::Expr_constContext *ctx) 
{
  return visitChildren(ctx);
}

Any Compiler::visitInitAssign(CoraxParser::InitAssignContext* ctx) 
{
  visitChildren(ctx);


  Result ass = results.get(ctx->expr_assi());
  if (!ass.isConst() && currentScope == globalTable.get())
  {
    string errmess = "attempting non-static calculation outside any function";
    addNodeError(ctx, errmess);
    Result res;
    res.setValue(0);
    results.put(ctx, res);
  }
  else
  {
    // Assign oper(ctx, currentScope, currentFunction, this);
    // Result lhs;
    // lhs.setValue(*currentId.back());
    // operation(ctx, lhs, results.get(ctx->expr_assi()), oper);
    results.put(ctx, ass);
  }

  return nullptr;
}

Any Compiler::visitInitDeclAssigned(CoraxParser::InitDeclAssignedContext* ctx)
{
  visit(ctx->declarator());
  // last identifier on the current scope is our boy
  Identifier& tempid = currentScope->GetLast();
  visit(ctx->initializer());

  // Right now this just assumes a simple declaration (not an initializer list)
  Assign oper(ctx, currentScope, currentFunction, this);
  Result lhs;
  lhs.setValue(tempid);
  operation(ctx, lhs, results.get(ctx->initializer()), oper);

  Result res;
  res.setValue(0);
  results.put(ctx, res);

  return nullptr;
}