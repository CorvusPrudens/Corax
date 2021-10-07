#ifndef ENCA_INSTRUCTION_H
#define ENCA_INSTRUCTION_H

#include <cstdint>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>

#include "antlr4-runtime.h"
#include "operand.h"
#include "symbols.h"
#include "error.h"

using namespace std;
using namespace antlr4;
using tree::ParseTree;

class Assembler; // forward decl

namespace assembler
{
  struct Machine {
    uint32_t size = 0;
    vector<uint8_t> bytes;
    void setBytes(uint32_t word, size_t num_bytes);
  };

  struct Mnemonic {
    Mnemonic(string n, uint32_t op, vector<vector<Operand::Class>> list);
    Mnemonic(string n, uint32_t op, uint32_t cond, vector<vector<Operand::Class>> list);
    Mnemonic(string n, uint32_t op) {
      name = n;
      opcode = op;
      conditional = op;
    }
    Mnemonic() {}
    ~Mnemonic() {}
    string name;
    uint32_t opcode;
    uint32_t conditional;
    vector<vector<Operand::Class>> validOperands;
  };

  struct Bits {
    Bits(uint32_t s, uint32_t m);

    void Set(uint32_t& word, uint32_t data) const;

    uint32_t shift;
    uint32_t mask;
  };

  #define M(name, opcode, ...) \
  {name, Mnemonic(name, opcode, {__VA_ARGS__})}

  #define MC(name, opcode, conditional, ...) \
  {name, Mnemonic(name, opcode, conditional, {__VA_ARGS__})}

  #define ARITHMETIC_OPERANDS \
  {Operand::REGISTER}, \
  {Operand::NAME, Operand::NUMBER, Operand::NUMBER_REL, Operand::REGISTER, Operand::REGISTER_REL}, \
  {Operand::REGISTER, Operand::NONE}

  class Instruction {
    public:

      Instruction(string mnem, ParseTree* c);
      ~Instruction() {}

      inline static unordered_map<string, Mnemonic> mnemonics = {
        M("nop", 0x00), 
        M("ldr", 0x01, {Operand::REGISTER}, {Operand::NAME, Operand::NUMBER, Operand::NUMBER_REL, Operand::REGISTER_REL}), 
        M("str", 0x02, {Operand::REGISTER}, {Operand::NAME, Operand::NUMBER_REL, Operand::REGISTER_REL}),
        M("mov", 0x05),
        M("cmp", 0x03, {Operand::REGISTER}, {Operand::NAME, Operand::NUMBER, Operand::NUMBER_REL, Operand::REGISTER, Operand::REGISTER_REL}), 
        M("cps", 0x04, {Operand::CONDITION}, ARITHMETIC_OPERANDS),
        MC("add", 0x05, 0x1E, ARITHMETIC_OPERANDS), 
        MC("sub", 0x06, 0x1D, ARITHMETIC_OPERANDS),
        M("mul", 0x07, ARITHMETIC_OPERANDS), 
        M("div", 0x08, ARITHMETIC_OPERANDS), 
        M("mod", 0x09, ARITHMETIC_OPERANDS), 
        M("and", 0x0A, ARITHMETIC_OPERANDS),
        M("or",  0x0B, ARITHMETIC_OPERANDS),  
        M("xor", 0x0C, ARITHMETIC_OPERANDS), 
        M("not", 0x0D, {Operand::NAME, Operand::NUMBER, Operand::NUMBER_REL, Operand::REGISTER, Operand::REGISTER_REL}, {Operand::REGISTER, Operand::NONE}), 
        M("lsl", 0x0E, ARITHMETIC_OPERANDS),
        M("lsr", 0x0F, ARITHMETIC_OPERANDS),
        MC("jmp", 0x10, 0x1F, {Operand::NAME, Operand::NUMBER, Operand::NUMBER_REL, Operand::REGISTER_REL}),
        M("push",0x11, {Operand::REGISTER}, {Operand::NUMBER, Operand::NONE}),
        M("pop", 0x12, {Operand::REGISTER}),
      };

      bool verifyOperands();
      void setMnemonic(string& mnem);
      void addOperand(Operand* op);
      void setCondition(Operand* cond) { condition = cond; }
      uint32_t GetSize(Assembler* ass);
      Machine& Assemble(Assembler* ass);
      
      void AddOpcode(uint32_t& code, Assembler* ass);
      void AddVariant(uint32_t& code, Operand* op, Assembler* ass);
      void AddRegisters(uint32_t& code, Operand* reg1, Operand* reg2, Operand* reg3, Assembler* ass);

      void AssembleNop(Assembler* ass, bool query);
      void AssembleLdr(Assembler* ass, bool query);
      void AssembleStr(Assembler* ass, bool query);
      void AssembleMov(Assembler* ass, bool query);

      void AssembleCmp(Assembler* ass, bool query);
      void AssembleCps(Assembler* ass, bool query);
      void AssembleAdd(Assembler* ass, bool query);
      void AssembleSub(Assembler* ass, bool query);

      void AssembleMul(Assembler* ass, bool query);
      void AssembleDiv(Assembler* ass, bool query);
      void AssembleMod(Assembler* ass, bool query);
      void AssembleAnd(Assembler* ass, bool query);

      void AssembleOr(Assembler* ass, bool query);
      void AssembleXor(Assembler* ass, bool query);
      void AssembleNot(Assembler* ass, bool query);
      void AssembleLsl(Assembler* ass, bool query);

      void AssembleLsr(Assembler* ass, bool query);
      void AssembleJmp(Assembler* ass, bool query);
      void AssemblePush(Assembler* ass, bool query);
      void AssemblePop(Assembler* ass, bool query);

      typedef void (Instruction::*AssembleInstr)(Assembler*, bool);

      inline static unordered_map<string, AssembleInstr> methods = {
        {"nop", &Instruction::AssembleNop},
        {"ldr", &Instruction::AssembleLdr},
        {"str", &Instruction::AssembleStr},
        {"mov", &Instruction::AssembleMov},
        {"cmp", &Instruction::AssembleCmp},
        {"cps", &Instruction::AssembleCps},
        {"add", &Instruction::AssembleAdd},
        {"sub", &Instruction::AssembleSub},
        {"mul", &Instruction::AssembleMul},
        {"div", &Instruction::AssembleDiv},
        {"mod", &Instruction::AssembleMod},
        {"and", &Instruction::AssembleAnd},
        {"or",  &Instruction::AssembleOr},
        {"xor", &Instruction::AssembleXor},
        {"not", &Instruction::AssembleNot},
        {"lsl", &Instruction::AssembleLsl},
        {"lsr", &Instruction::AssembleLsr},
        {"jmp", &Instruction::AssembleJmp},
        {"push",&Instruction::AssemblePush},
        {"pop", &Instruction::AssemblePop},
      };

      inline static const Bits OpcodeBits{0, 0b11111};
      inline static const Bits VariantBits{5, 0b11};
      inline static const Bits Operand1Bits{7, 0b111};
      inline static const Bits Operand2Bits{10, 0b111};
      inline static const Bits ResultsBits{13, 0b111};
      inline static const Bits Word2Bits{16, 0xFFFF};

      string mnemonic;
      vector<Operand*> operands;
      Operand* condition;
      ParseTree* ctx;
      Machine machine;
  };
};

#endif