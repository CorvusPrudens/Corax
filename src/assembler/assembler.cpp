
#include "assembler.h"
#include "number.h"

Assembler::Assembler(string filename)
: input(filename)
, stream(input)
, lexer(&stream)
, tokens(&lexer)
, parser(&tokens)
, err(filename, &tokens)
{

  tree::ParseTree* parse_tree = parser.parse();
  visit(parse_tree);

  size_t address = 0;
  for (int i = 0; i < instructions.size(); i++) {
    for (auto& symbol : symbols.symbols) {
      if (symbol.second.isLabel && symbol.second.index == i) {
        symbol.second.address = address;
      }
    }
    address += instructions[i].GetSize(&err);
  }

  for (auto& instruction : instructions)
  {
    Machine& temp = instruction.Assemble(&err);
    for (auto byte : temp.bytes) {
      machine_code.push_back(byte);
    }
  }
}

void Assembler::Complete()
{
  err.Report();
}

Any Assembler::visitStatLab(EncaParser::StatLabContext* ctx)
{
  Symbol s(ctx->label()->NAME()->getText(), instructions.size());
  try {
    symbols.AddSymbol(s, &err, ctx);
  } catch (int e) {
    string errmess = "label already defined";
    err.AddNodeErr(errmess, ctx);
  }
  
  return nullptr;
}

// TODO -- just compress this into one function!
Any Assembler::visitInstr(EncaParser::InstrContext* ctx)
{
  Instruction inst(ctx->mnemonic()->getText(), ctx);
  instructions.push_back(inst);
  return nullptr;
}
Any Assembler::visitInstrOper(EncaParser::InstrOperContext* ctx)
{
  Instruction inst(ctx->mnemonic()->getText(), ctx);
  visitChildren(ctx);
  for (auto ptr : ctx->operands()->operand()) {
    Operand* op = operands.get(ptr);
    inst.addOperand(op);
  }
  instructions.push_back(inst);
  return nullptr;
}
Any Assembler::visitInstrCond(EncaParser::InstrCondContext* ctx)
{
  Instruction inst(ctx->mnemonic()->getText(), ctx);
  visitChildren(ctx);
  inst.setCondition(operands.get(ctx->conditional()));
  instructions.push_back(inst);
  return nullptr;
}
Any Assembler::visitInstrOperCond(EncaParser::InstrOperCondContext* ctx)
{
  Instruction inst(ctx->mnemonic()->getText(), ctx);
  visitChildren(ctx);
  for (auto ptr : ctx->operands()->operand()) {
    Operand* op = operands.get(ptr);
    inst.addOperand(op);
  }
  inst.setCondition(operands.get(ctx->conditional()));
  instructions.push_back(inst);
  return nullptr;
}

Any Assembler::visitOpReg(EncaParser::OpRegContext* ctx)
{
  CREATE_OP(RegisterOp, ctx->reg()->getText())
  return nullptr;
}

Any Assembler::visitOpNum(EncaParser::OpNumContext* ctx)
{
  visit(ctx->number());
  CREATE_OP(NumberOp, numbers.get(ctx->number()))
  return nullptr;
}
Any Assembler::visitOpCond(EncaParser::OpCondContext* ctx)
{
  CREATE_OP(ConditionOp, ctx->condition()->getText())
  return nullptr;
}
Any Assembler::visitOpVar(EncaParser::OpVarContext* ctx)
{
  visit(ctx->variable());
  CREATE_OP(
    VariableOp, 
    values.get(ctx->variable()).reference.name, 
    &symbols, 
    values.get(ctx->variable()).reference.isAddress
  )
  return nullptr;
}
Any Assembler::visitOpRel(EncaParser::OpRelContext* ctx)
{
  visitChildren(ctx);
  auto op = operands.get(ctx->operand());
  uint16_t offset = 0;
  if (ctx->number() != nullptr) {
    offset = numbers.get(ctx->number()).getValue();
  }
  op->setRelative(offset);
  operands.put(ctx, operands.getPtr(ctx->operand()));
  return nullptr;
}

Any Assembler::visitConditional(EncaParser::ConditionalContext* ctx)
{
  CREATE_OP(ConditionOp, ctx->condition()->getText())
  return nullptr;
}

Any Assembler::visitNumDec(EncaParser::NumDecContext *ctx) {
  uint16_t value = stoi(ctx->getText());
  Number num;
  num.setValue(value, Number::Type::INT);
  numbers.put(ctx, num);
  return nullptr;
}

Any Assembler::visitNumFlt(EncaParser::NumFltContext *ctx) {
  float value = stof(ctx->getText());
  Number num;
  num.setValue(value, Number::Type::FLT);
  numbers.put(ctx, num);
  return nullptr;
}

Any Assembler::visitNumSci(EncaParser::NumSciContext *ctx) {
  float value = stof(ctx->getText());
  Number num;
  num.setValue(value, Number::Type::FLT);
  numbers.put(ctx, num);
  return nullptr;
}

Any Assembler::visitNumHex(EncaParser::NumHexContext *ctx) {
  uint16_t value = 0;
  Number num;
  try {
    value = stoi(ctx->getText(), nullptr, 16);
  } catch (std::out_of_range e) {
    string mess = "literal out of range of 16-bit word";
    err.AddNodeErr(mess, ctx);
  }
  num.setValue(value, Number::Type::INT);
  numbers.put(ctx, num);
  return nullptr;
}

Any Assembler::visitNumBin(EncaParser::NumBinContext *ctx) {
  uint16_t value = stoi(ctx->getText(), nullptr, 2);
  Number num;
  num.setValue(value, Number::Type::INT);
  numbers.put(ctx, num);
  return nullptr;
}

Any Assembler::visitNumOct(EncaParser::NumOctContext *ctx) {
  uint16_t value = stoi(ctx->getText(), nullptr, 8);
  Number num;
  num.setValue(value, Number::Type::INT);
  numbers.put(ctx, num);
  return nullptr;
}

Any Assembler::visitDataArray(EncaParser::DataArrayContext* ctx) 
{
  int dimensions = visit(ctx->storage()).as<int>();
  if (dimensions == 0) {
    string errmess = "invalid dimension";
    err.AddNodeErr(errmess, ctx->storage()->dimension());
    dimensions = 1;
  }
  Symbol sym = data_symbols.get(ctx->storage());
  
  if (ctx->data_list() != nullptr) {
    for (auto element : ctx->data_list()->data_element()) {
      visit(element);
      if (dimensions != -1 && sym.data.size() > dimensions) {
        string errmess = "initializer list larger than specified dimensions";
        err.AddNodeErr(errmess, element);
      }
      else
      {
        sym.data.push_back(values.get(element));
      }
    }
  }

  if (dimensions == -1 && sym.data.size() == 0) {
    string errmess = "ambiguous data size";
    err.AddNodeErr(errmess, ctx->storage()->NAME());
    sym.data.push_back(Value(0));
  }
  else
  {
    if (dimensions != -1) {
      while (sym.data.size() < dimensions) {
        sym.data.push_back(Value(0));
      }
    }
    
  }
  
  symbols.AddSymbol(sym, &err, ctx->storage()->NAME());
  return nullptr;
}

Any Assembler::visitDataSingle(EncaParser::DataSingleContext* ctx) 
{
  int dimensions = visit(ctx->storage()).as<int>();
  visit(ctx->data_element());

  if (dimensions != -1 && dimensions != 1) {
    string errmess = "invalid dimension for data without initializer list";
    err.AddNodeErr(errmess, ctx->storage()->dimension());
  }
  Symbol sym = data_symbols.get(ctx->storage());
  sym.data.push_back(values.get(ctx->data_element()));
  symbols.AddSymbol(sym, &err, ctx->storage()->NAME());

  return nullptr;
}
Any Assembler::visitStorage(EncaParser::StorageContext* ctx) 
{
  Symbol sym(ctx->NAME()->getText());
  if (ctx->specifier_list() != nullptr) {
    visit(ctx->specifier_list());
    sym.attributes = attributes.get(ctx->specifier_list());
  }
  data_symbols.put(ctx, sym);
  return visit(ctx->dimension());
}
Any Assembler::visitSpecifier_list(EncaParser::Specifier_listContext* ctx)
{
  Attributes attr;
  for (auto item : ctx->specifier()) {
    int value = 0;
    if (item->number() != nullptr)
    {
      visit(item->number());
      value = numbers.get(item->number()).getValue();
    }
    attr[item->NAME()->getText()] = value;
  }
  attributes.put(ctx, attr);
  return nullptr;
}
Any Assembler::visitDimEmpty(EncaParser::DimEmptyContext* ctx)
{
  return -1;
}
Any Assembler::visitDimNumber(EncaParser::DimNumberContext* ctx)
{
  visitChildren(ctx);
  int dimensions = numbers.get(ctx->number()).getValue();
  if (ctx->number()->getText()[0] == '-') {
    err.AddNodeErr("negative dimensions are not permitted", ctx->number());
  }
  return dimensions;
}
Any Assembler::visitVar(EncaParser::VarContext* ctx)
{
  Reference r(ctx->NAME()->getText());
  Value v(r);
  values.put(ctx, v);
  return nullptr;
}
Any Assembler::visitVarAddr(EncaParser::VarAddrContext* ctx)
{
  Reference r(ctx->NAME()->getText());
  r.isAddress = true;
  Value v(r);
  values.put(ctx, v);
  return nullptr;
}

Any Assembler::visitDataNumber(EncaParser::DataNumberContext* ctx)
{
  visit(ctx->number());
  Value v(numbers.get(ctx->number()).getValue());
  values.put(ctx, v);
  return nullptr;
}
Any Assembler::visitDataVariable(EncaParser::DataVariableContext* ctx)
{
  visit(ctx->variable());
  values.put(ctx, values.get(ctx->variable()));
  return nullptr;
}
