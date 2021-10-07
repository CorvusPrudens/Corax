
#include "instruction.h"
#include "error.h"
#include "assembler.h"

// NOTE -- if the conditional opcode is the same as the non-conditional one, then the mnemonic doesn't have a conditional version

assembler::Mnemonic::Mnemonic(string n, uint32_t op, vector<vector<Operand::Class>> list)  
{
  name = n;
  opcode = op;
  conditional = op;

  for (auto& item : list) {
    validOperands.push_back(item);
  }
}

assembler::Mnemonic::Mnemonic(string n, uint32_t op, uint32_t cond, vector<vector<Operand::Class>> list) 
{
  name = n;
  opcode = op;
  conditional = cond;

  for (auto& item : list) {
    validOperands.push_back(item);
  }
}

assembler::Bits::Bits(uint32_t s, uint32_t m)
{
  shift = s;
  mask = m << shift;
}

void assembler::Bits::Set(uint32_t& word, uint32_t data) const
{
  word &= ~mask;
  word |= (data << shift) & mask;
}

void assembler::Machine::setBytes(uint32_t word, size_t num_bytes)
{
  for (int i = 0; i < num_bytes; i++)
  {
    uint8_t byte = word >> (8 * i);
    bytes.push_back(byte);
  }
}

assembler::Instruction::Instruction(string mnem, ParseTree* c)
{
  // string* idx = std::find(mnemonics, mnemonics + OP_COUNT, mnem);
  // if (idx == mnemonics + OP_COUNT) throw mnem;

  // index = (Index) (idx - mnemonics);
  // ctx = c;
  condition = nullptr;
  mnemonic = mnem;
  ctx = c;
}

bool op_in(Operand::Class c, vector<Operand::Class> classes) {
  for (auto& cls : classes)
    if (c == cls) return true;
  return false;
}

bool assembler::Instruction::verifyOperands() 
{
  auto& valid = mnemonics[mnemonic].validOperands;
  if (operands.size() > valid.size())
    return false;
  
  for (int i = 0; i < valid.size(); i++) 
  {
    if (i >= operands.size()) 
    {
      if (!op_in(Operand::NONE, valid[i]))
        return false;
    }
    else
    {
      if (!op_in(operands[i]->getClass(), valid[i]))
        return false;
    }
  }

  return true;
}

void assembler::Instruction::addOperand(Operand* op) {
  operands.push_back(op);
}

uint32_t assembler::Instruction::GetSize(Assembler* ass)
{
  if (methods.count(mnemonic) > 0) { 
    (this->*methods[mnemonic])(ass, true);
    return machine.size;
  } else { 
    return 0;
  } 
}

void assembler::Instruction::AddOpcode(uint32_t& code, Assembler* ass)
{
  if (condition != nullptr)
  {
    if (mnemonics[mnemonic].opcode == mnemonics[mnemonic].conditional)
    {
      code = 0;
      string errmess = "\"" + mnemonic + "\" does not support conditional execution";
      ass->addNodeError(ctx, errmess);
    }
    else
    {
      OpcodeBits.Set(code, mnemonics[mnemonic].conditional);
      // code &= ~0b11111;
      // code |= mnemonics[mnemonic].conditional;
    }
  }
  else
  {
    OpcodeBits.Set(code, mnemonics[mnemonic].opcode);
    // code &= ~0b11111;
    // code |= mnemonics[mnemonic].opcode;
  }
}

void assembler::Instruction::AddVariant(uint32_t& code, Operand* op, Assembler* ass)
{
  switch (op->getClass())
  {
    default:
    case Operand::REGISTER:
      VariantBits.Set(code, 0b00);
      break;
    case Operand::NAME:
    case Operand::NUMBER_REL:
      VariantBits.Set(code, 0b01);
      break;
    case Operand::NUMBER:
      VariantBits.Set(code, 0b10);
      break;
    case Operand::REGISTER_REL:
      VariantBits.Set(code, 0b11);
      break;
    case Operand::NAME_REL:
    case Operand::CONDITION_REL:
      {
        string errmess = "invalid relative operand";
        ass->addNodeError(ctx, errmess);
      }
      break;
  }
}

void assembler::Instruction::AddRegisters(uint32_t& code, Operand* reg1, Operand* reg2, Operand* reg3, Assembler* ass)
{
  uint32_t r1 = reg1 == nullptr ? 0b000 : reg1->getValue();
  uint32_t r2 = reg2 == nullptr ? 0b000 : reg2->getValue();
  uint32_t r3 = reg3 == nullptr ? 0b000 : reg3->getValue();

  Operand1Bits.Set(code, r1);
  Operand2Bits.Set(code, r2);
  ResultsBits.Set(code, r3);
}

assembler::Machine& assembler::Instruction::Assemble(Assembler* ass) 
{ 
  if (methods.count(mnemonic) > 0) { 
    if (!verifyOperands())
    {
      string errmess = "invalid operand";
      ass->addNodeError(ctx, errmess);
    }
    else
    {
      try { 
        (this->*methods[mnemonic])(ass, false); 
      } catch (int e) { 
        ass->addNodeError(ctx, "malformed arguments"); 
      } 
    }
  } else { 
    ass->addNodeError(ctx, "unexpected mnemonic \"" + mnemonic + "\""); 
  } 
  return machine;
}

#define WORD_BYTES 2

void assembler::Instruction::AssembleNop(Assembler* ass, bool query)
{
  if (query)
    machine.size = WORD_BYTES;
  else
    machine.bytes = {0, 0};
}
void assembler::Instruction::AssembleLdr(Assembler* ass, bool query)
{
  if (query) {
    machine.size = WORD_BYTES * 2;
  } else {
    uint32_t code = 0;
    AddOpcode(code, ass);
    AddVariant(code, operands[1], ass);

    Operand* ptr = nullptr;
    if (operands[1]->getClass() == Operand::REGISTER_REL) 
    {
      ptr = operands[1];
      Word2Bits.Set(code, operands[1]->relativeOffset);
    }
    else
    {
      Word2Bits.Set(code, operands[1]->getValue());
    }

    AddRegisters(code, nullptr, ptr, operands[0], ass);
    machine.setBytes(code, machine.size);
  }
}
void assembler::Instruction::AssembleStr(Assembler* ass, bool query)
{
  if (query) {
    machine.size = WORD_BYTES * 2;
  } else {
    uint32_t code = 0;
    AddOpcode(code, ass);
    AddVariant(code, operands[1], ass);

    Operand* ptr = nullptr;
    if (operands[1]->getClass() == Operand::REGISTER_REL) 
    {
      ptr = operands[1];
      Word2Bits.Set(code, operands[1]->relativeOffset);
    }
    else
    {
      Word2Bits.Set(code, operands[1]->getValue());
    }

    AddRegisters(code, nullptr, ptr, operands[0], ass);
    machine.setBytes(code, machine.size);
  }
}

// not done (or sure if it's gonna be used lol)
void assembler::Instruction::AssembleMov(Assembler* ass, bool query)
{
  if (query) {

    machine.size = WORD_BYTES;
  } else {
    uint32_t code = 0;
    AddOpcode(code, ass);
    machine.setBytes(code, machine.size);
  }
}

#define TYPICAL_SIZE(opnum) \
switch (operands[opnum]->getClass()) \
{ \
  case Operand::Class::REGISTER: \
    machine.size = WORD_BYTES; \
    break; \
  default: \
    machine.size = WORD_BYTES * 2; \
    break; \
}

void assembler::Instruction::AssembleCmp(Assembler* ass, bool query)
{
  if (query) {
    TYPICAL_SIZE(1)
  } else {
    uint32_t code = 0;
    AddOpcode(code, ass);
    AddVariant(code, operands[1], ass);

    Operand* op2 = nullptr;

    if (operands[1]->getClass() == Operand::REGISTER_REL) 
    {
      op2 = operands[1];
      Word2Bits.Set(code, operands[1]->relativeOffset);
    }
    else if (operands[1]->getClass() == Operand::REGISTER)
      op2 = operands[1];
    else
      Word2Bits.Set(code, operands[1]->getValue());

    AddRegisters(code, operands[0], op2, nullptr, ass);
    machine.setBytes(code, machine.size);
  }
}

// TODO -- Not ready yet (3 word instruction)
void assembler::Instruction::AssembleCps(Assembler* ass, bool query)
{
  if (query) 
  {
    switch (operands[2]->getClass()) 
    { 
      case Operand::Class::REGISTER: 
        machine.size = WORD_BYTES * 2; 
        break; 
      default:
        machine.size = WORD_BYTES * 3;
        break;
    }
  } 
  else 
  {
    uint32_t code = 0;
    AddOpcode(code, ass);
    AddVariant(code, operands[1], ass);
    machine.setBytes(code, machine.size);
  }
}

// TODO -- this should be replaced with an actual function call

#define TYPICAL_ARITHMETIC \
if (query) { \
  TYPICAL_SIZE(1) \
} else { \
  uint32_t code = 0; \
  AddOpcode(code, ass); \
  AddVariant(code, operands[1], ass); \
 \
  Operand* op2 = nullptr; \
  Operand* results = operands.size() < 3 ? operands[0] : operands[2]; \
 \
  if (operands[1]->getClass() == Operand::REGISTER_REL) \
  { \
    op2 = operands[1]; \
    Word2Bits.Set(code, operands[1]->relativeOffset); \
  } \
  else if (operands[1]->getClass() == Operand::REGISTER) \
    op2 = operands[1]; \
  else \
    Word2Bits.Set(code, operands[1]->getValue()); \
 \
  AddRegisters(code, operands[0], op2, results, ass); \
  machine.setBytes(code, machine.size); \
}

void assembler::Instruction::AssembleAdd(Assembler* ass, bool query)
{
  TYPICAL_ARITHMETIC
}
void assembler::Instruction::AssembleSub(Assembler* ass, bool query)
{
  TYPICAL_ARITHMETIC
}

void assembler::Instruction::AssembleMul(Assembler* ass, bool query)
{
  TYPICAL_ARITHMETIC
}
void assembler::Instruction::AssembleDiv(Assembler* ass, bool query)
{
  TYPICAL_ARITHMETIC
}
void assembler::Instruction::AssembleMod(Assembler* ass, bool query)
{
  TYPICAL_ARITHMETIC
}
void assembler::Instruction::AssembleAnd(Assembler* ass, bool query)
{
  TYPICAL_ARITHMETIC
}

void assembler::Instruction::AssembleOr(Assembler* ass, bool query)
{
  TYPICAL_ARITHMETIC
}
void assembler::Instruction::AssembleXor(Assembler* ass, bool query)
{
  TYPICAL_ARITHMETIC
}

void assembler::Instruction::AssembleNot(Assembler* ass, bool query)
{
  if (query) {
    TYPICAL_SIZE(0)
  } else {
    uint32_t code = 0;
    AddOpcode(code, ass);
    AddVariant(code, operands[0], ass);

    Operand* ptr = nullptr;
    Operand* results = nullptr;
    if (operands[0]->getClass() == Operand::REGISTER_REL) 
    {
      ptr = operands[0];
      Word2Bits.Set(code, operands[0]->relativeOffset);
      results = operands[1];
    }
    else if (operands[0]->getClass() == Operand::REGISTER)
    {
      ptr = operands[0];
      results = operands.size() > 1 ? operands[1] : operands[0];
    }
    else
    {
      Word2Bits.Set(code, operands[0]->getValue());
      results = operands[1];
    }

    AddRegisters(code, nullptr, ptr, results, ass);
    machine.setBytes(code, machine.size);
  }
}
void assembler::Instruction::AssembleLsl(Assembler* ass, bool query)
{
  TYPICAL_ARITHMETIC
}

void assembler::Instruction::AssembleLsr(Assembler* ass, bool query)
{
  TYPICAL_ARITHMETIC
}
void assembler::Instruction::AssembleJmp(Assembler* ass, bool query)
{
   if (query) {
    machine.size = WORD_BYTES * 2;
  } else {
    uint32_t code = 0;
    AddOpcode(code, ass);
    AddVariant(code, operands[0], ass);

    Operand* ptr = nullptr;
    if (operands[0]->getClass() == Operand::REGISTER_REL) 
    {
      ptr = operands[0];
      Word2Bits.Set(code, operands[0]->relativeOffset);
    }
    else
    {
      Word2Bits.Set(code, operands[0]->getValue());
    }

    AddRegisters(code, nullptr, ptr, nullptr, ass);

    if (condition != nullptr)
    {
      uint32_t cond_encoding = condition->getValue();
      Operand1Bits.Set(code, cond_encoding);
      ResultsBits.Set(code, cond_encoding >> 3);
    }

    machine.setBytes(code, machine.size);
  }
}
void assembler::Instruction::AssemblePush(Assembler* ass, bool query)
{
  if (query) {
    machine.size = WORD_BYTES;
    switch (operands[1]->getClass()) 
    { 
      default:
      case Operand::Class::NONE: 
        machine.size = WORD_BYTES; 
        break; 
      case Operand::Class::NUMBER:
        machine.size = WORD_BYTES * 2;
        break;
    }
  } else {
    uint32_t code = 0;
    AddOpcode(code, ass);
    AddRegisters(code, nullptr, nullptr, operands[0], ass);
    if (operands[1]->getClass() == Operand::Class::NUMBER)
    {
      AddVariant(code, operands[1], ass);
      Word2Bits.Set(code, operands[1]->getValue());
    }
    machine.setBytes(code, machine.size);
  }
}

void assembler::Instruction::AssemblePop(Assembler* ass, bool query)
{
  if (query) {
    machine.size = WORD_BYTES;
  } else {
    uint32_t code = 0;
    AddOpcode(code, ass);
    AddRegisters(code, nullptr, nullptr, operands[0], ass);
    machine.setBytes(code, machine.size);
  }
}