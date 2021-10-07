
#ifndef ENCA_ASSEMBLER_H
#define ENCA_ASSEMBLER_H

#include <fstream>
#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

#include "antlr4-runtime.h"
#include "EncaLexer.h"
#include "EncaParser.h"
#include "EncaBaseVisitor.h"
#include "EncaBaseListener.h"

#include "error.h"
#include "operand.h"
#include "number.h"
#include "instruction.h"
#include "symbols.h"

using namespace antlr4;
using antlrcpp::Any;
using namespace std;

#define CREATE_OP(cname, ...) \
unique_ptr<Operand> op = make_unique<cname>(__VA_ARGS__); \
operands.put(ctx, &op);

template <typename T>
class PtrProperty {

  public:
    void put(tree::ParseTree* ctx, unique_ptr<T>* ptr) {
      properties[ctx] = move(*ptr);
    }
    T* get(tree::ParseTree* ctx) {
      return properties[ctx].get();
    }
    unique_ptr<T>* getPtr(tree::ParseTree* ctx) {
      return &properties[ctx];
    }

  unordered_map<tree::ParseTree*, unique_ptr<T>> properties;
};

struct Kwarg {
  Kwarg() {}
  Kwarg(string n, int v) { name = n; value = v; }
  string name;
  int value;
};

class Assembler : public EncaBaseVisitor {

  public:
    Assembler(string file_);
    ~Assembler() {}

    
    SymbolTable symbols;
    PtrProperty<Operand> operands;
    // unordered_map<tree::ParseTree*, unique_ptr<Operand>> operands;
    // tree::ParseTreeProperty<Operand> operands;
    tree::ParseTreeProperty<Number> numbers;
    tree::ParseTreeProperty<Symbol> data_symbols;
    tree::ParseTreeProperty<Value> values;
    tree::ParseTreeProperty<unordered_map<string, int>> attributes;
    vector<Instruction> instructions;
    vector<uint8_t> machine_code;

    fstream input;
    ANTLRInputStream stream;
    EncaLexer lexer;
    EncaParser parser;
    CommonTokenStream tokens;
    Error err;

    void Complete();

    Any visitInstr(EncaParser::InstrContext* ctx) override;
    Any visitInstrOper(EncaParser::InstrOperContext* ctx) override;
    Any visitInstrCond(EncaParser::InstrCondContext* ctx) override;
    Any visitInstrOperCond(EncaParser::InstrOperCondContext* ctx) override;

    Any visitOpReg(EncaParser::OpRegContext* ctx) override;
    Any visitOpNum(EncaParser::OpNumContext* ctx) override;
    Any visitOpCond(EncaParser::OpCondContext* ctx) override;
    Any visitOpVar(EncaParser::OpVarContext* ctx) override;
    Any visitOpRel(EncaParser::OpRelContext* ctx) override;

    Any visitConditional(EncaParser::ConditionalContext* ctx) override;

    Any visitNumDec(EncaParser::NumDecContext *ctx) override;
    Any visitNumFlt(EncaParser::NumFltContext *ctx) override;
    Any visitNumSci(EncaParser::NumSciContext *ctx) override;
    Any visitNumHex(EncaParser::NumHexContext *ctx) override;
    Any visitNumBin(EncaParser::NumBinContext *ctx) override;
    Any visitNumOct(EncaParser::NumOctContext *ctx) override;

    Any visitStatLab(EncaParser::StatLabContext* ctx) override;

    Any visitDataArray(EncaParser::DataArrayContext* ctx) override;
    Any visitDataSingle(EncaParser::DataSingleContext* ctx) override;
    Any visitSpecifier_list(EncaParser::Specifier_listContext* ctx) override;

    Any visitStorage(EncaParser::StorageContext* ctx) override;
    Any visitDimEmpty(EncaParser::DimEmptyContext* ctx) override;
    Any visitDimNumber(EncaParser::DimNumberContext* ctx) override;

    Any visitVar(EncaParser::VarContext* ctx) override;
    Any visitVarAddr(EncaParser::VarAddrContext* ctx) override;

    Any visitDataNumber(EncaParser::DataNumberContext* ctx) override;
    Any visitDataVariable(EncaParser::DataVariableContext* ctx) override;

};

#endif