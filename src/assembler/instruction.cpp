
#include "instruction.h"
#include "error.h"

// NOTE -- if the conditional opcode is the same as the non-conditional one, then the mnemonic doesn't have a conditional version

Mnemonic::Mnemonic(string n, uint32_t op, vector<vector<Operand::Class>> list)  
{
  name = n;
  opcode = op;
  conditional = op;

  for (auto& item : list) {
    validOperands.push_back(item);
  }
}

Mnemonic::Mnemonic(string n, uint32_t op, uint32_t cond, vector<vector<Operand::Class>> list) 
{
  name = n;
  opcode = op;
  conditional = cond;

  for (auto& item : list) {
    validOperands.push_back(item);
  }
}

Bits::Bits(uint32_t s, uint32_t m)
{
  shift = s;
  mask = m << shift;
}

void Bits::Set(uint32_t& word, uint32_t data) const
{
  word &= ~mask;
  word |= (data << shift) & mask;
}

void Machine::setBytes(uint32_t word, size_t num_bytes)
{
  for (int i = 0; i < num_bytes; i++)
  {
    uint8_t byte = word >> (8 * i);
    bytes.push_back(byte);
  }
}

Instruction::Instruction(string mnem, ParseTree* c)
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

bool Instruction::verifyOperands() 
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

void Instruction::addOperand(Operand* op) {
  operands.push_back(op);
}

uint32_t Instruction::GetSize(Error* err)
{
  if (methods.count(mnemonic) > 0) { 
    (this->*methods[mnemonic])(err, true);
    return machine.size;
  } else { 
    return 0;
  } 
}

void Instruction::AddOpcode(uint32_t& code, Error* err)
{
  if (condition != nullptr)
  {
    if (mnemonics[mnemonic].opcode == mnemonics[mnemonic].conditional)
    {
      code = 0;
      string errmess = "\"" + mnemonic + "\" does not support conditional execution";
      err->AddNodeErr(errmess, ctx);
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

void Instruction::AddVariant(uint32_t& code, Operand* op, Error* err)
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
        err->AddNodeErr(errmess, ctx);
      }
      break;
  }
}

void Instruction::AddRegisters(uint32_t& code, Operand* reg1, Operand* reg2, Operand* reg3, Error* err)
{
  uint32_t r1 = reg1 == nullptr ? 0b000 : reg1->getValue();
  uint32_t r2 = reg2 == nullptr ? 0b000 : reg2->getValue();
  uint32_t r3 = reg3 == nullptr ? 0b000 : reg3->getValue();

  Operand1Bits.Set(code, r1);
  Operand2Bits.Set(code, r2);
  ResultsBits.Set(code, r3);
}

Machine& Instruction::Assemble(Error* err) 
{ 
  if (methods.count(mnemonic) > 0) { 
    if (!verifyOperands())
    {
      string errmess = "invalid operand";
      err->AddNodeErr(errmess, ctx);
    }
    else
    {
      try { 
        (this->*methods[mnemonic])(err, false); 
      } catch (int e) { 
        err->AddNodeErr("malformed arguments", ctx); 
      } 
    }
  } else { 
    err->AddNodeErr("unexpected mnemonic \"" + mnemonic + "\"", ctx); 
  } 
  return machine;
}

#define WORD_BYTES 2

void Instruction::AssembleNop(Error* err, bool query)
{
  if (query)
    machine.size = WORD_BYTES;
  else
    machine.bytes = {0, 0};
}
void Instruction::AssembleLdr(Error* err, bool query)
{
  if (query) {
    machine.size = WORD_BYTES * 2;
  } else {
    uint32_t code = 0;
    AddOpcode(code, err);
    AddVariant(code, operands[1], err);

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

    AddRegisters(code, nullptr, ptr, operands[0], err);
    machine.setBytes(code, machine.size);
  }
}
void Instruction::AssembleStr(Error* err, bool query)
{
  if (query) {
    machine.size = WORD_BYTES * 2;
  } else {
    uint32_t code = 0;
    AddOpcode(code, err);
    AddVariant(code, operands[1], err);

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

    AddRegisters(code, nullptr, ptr, operands[0], err);
    machine.setBytes(code, machine.size);
  }
}

// not done (or sure if it's gonna be used lol)
void Instruction::AssembleMov(Error* err, bool query)
{
  if (query) {

    machine.size = WORD_BYTES;
  } else {
    uint32_t code = 0;
    AddOpcode(code, err);
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

void Instruction::AssembleCmp(Error* err, bool query)
{
  if (query) {
    TYPICAL_SIZE(1)
  } else {
    uint32_t code = 0;
    AddOpcode(code, err);
    AddVariant(code, operands[1], err);

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

    AddRegisters(code, operands[0], op2, nullptr, err);
    machine.setBytes(code, machine.size);
  }
}

// TODO -- Not ready yet (3 word instruction)
void Instruction::AssembleCps(Error* err, bool query)
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
    AddOpcode(code, err);
    AddVariant(code, operands[1], err);
    machine.setBytes(code, machine.size);
  }
}

// TODO -- this should be replaced with an actual function call

#define TYPICAL_ARITHMETIC \
if (query) { \
  TYPICAL_SIZE(1) \
} else { \
  uint32_t code = 0; \
  AddOpcode(code, err); \
  AddVariant(code, operands[1], err); \
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
  AddRegisters(code, operands[0], op2, results, err); \
  machine.setBytes(code, machine.size); \
}

void Instruction::AssembleAdd(Error* err, bool query)
{
  TYPICAL_ARITHMETIC
}
void Instruction::AssembleSub(Error* err, bool query)
{
  TYPICAL_ARITHMETIC
}

void Instruction::AssembleMul(Error* err, bool query)
{
  TYPICAL_ARITHMETIC
}
void Instruction::AssembleDiv(Error* err, bool query)
{
  TYPICAL_ARITHMETIC
}
void Instruction::AssembleMod(Error* err, bool query)
{
  TYPICAL_ARITHMETIC
}
void Instruction::AssembleAnd(Error* err, bool query)
{
  TYPICAL_ARITHMETIC
}

void Instruction::AssembleOr(Error* err, bool query)
{
  TYPICAL_ARITHMETIC
}
void Instruction::AssembleXor(Error* err, bool query)
{
  TYPICAL_ARITHMETIC
}

void Instruction::AssembleNot(Error* err, bool query)
{
  if (query) {
    TYPICAL_SIZE(0)
  } else {
    uint32_t code = 0;
    AddOpcode(code, err);
    AddVariant(code, operands[0], err);

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

    AddRegisters(code, nullptr, ptr, results, err);
    machine.setBytes(code, machine.size);
  }
}
void Instruction::AssembleLsl(Error* err, bool query)
{
  TYPICAL_ARITHMETIC
}

void Instruction::AssembleLsr(Error* err, bool query)
{
  TYPICAL_ARITHMETIC
}
void Instruction::AssembleJmp(Error* err, bool query)
{
   if (query) {
    machine.size = WORD_BYTES * 2;
  } else {
    uint32_t code = 0;
    AddOpcode(code, err);
    AddVariant(code, operands[0], err);

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

    AddRegisters(code, nullptr, ptr, nullptr, err);

    if (condition != nullptr)
    {
      uint32_t cond_encoding = condition->getValue();
      Operand1Bits.Set(code, cond_encoding);
      ResultsBits.Set(code, cond_encoding >> 3);
    }

    machine.setBytes(code, machine.size);
  }
}
void Instruction::AssemblePush(Error* err, bool query)
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
    AddOpcode(code, err);
    AddRegisters(code, nullptr, nullptr, operands[0], err);
    if (operands[1]->getClass() == Operand::Class::NUMBER)
    {
      AddVariant(code, operands[1], err);
      Word2Bits.Set(code, operands[1]->getValue());
    }
    machine.setBytes(code, machine.size);
  }
}

void Instruction::AssemblePop(Error* err, bool query)
{
  if (query) {
    machine.size = WORD_BYTES;
  } else {
    uint32_t code = 0;
    AddOpcode(code, err);
    AddRegisters(code, nullptr, nullptr, operands[0], err);
    machine.setBytes(code, machine.size);
  }
}