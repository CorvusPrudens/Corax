#include <limits>
#include "target.h"

void FuncTrans::AddLine(Line line, Instruction& inst, vector<Register*> regs)
{
  instruction_reference[&inst].push_back(line);
  for (auto r : regs)
    used_registers.emplace(r);
}

void FuncTrans::PrependLine(Line line, Instruction& inst, vector<Register*> regs)
{
  instruction_reference[&inst].insert(instruction_reference[&inst].begin(),line);
  for (auto r : regs)
    used_registers.emplace(r);
}

void FuncTrans::Flatten()
{
  for (auto& inst : *instructions)
  {
    for (auto& line : instruction_reference[&inst])
    {
      translation.push_back(line);
    }
  }
}

Instruction* FuncTrans::GetSetupInstruction()
{
  for (auto& inst : *instructions)
  {
    if (inst.instr == Instruction::SETUP)
      return &inst;
  }
  throw 1;
}

void FuncTrans::AssignArgumentOffsets(Identifier& function)
{
  int offset = -2; // TODO -- remove this magic number
  for (auto& arg : function.members)
  {
    auto idx = &function.funcTable->GetLocalSymbol(arg.name);
    stack_offsets[idx] = offset;
    offset -= idx->dataType.size();
  }
}

// TODO -- this is also annoyingly target dependent... ABSTRACT!!!
void FuncTrans::AllocateVariables(Register* stack_pointer)
{
  for (auto& inst : *instructions)
  {
    size_t size = 0;
    if (inst.instr == Instruction::SCOPE_BEGIN)
    {
      int parent_stack_offset;
      if (inst.scope->scope != SymbolTable::FUNCTION)
        parent_stack_offset = inst.scope->parent->stack_offset;
      else
        // the base pointer points to the return address by default (right?), so we need an offset
        // TODO -- remove this magic number here too
        parent_stack_offset = 4;

      auto& identifiers = allocated_variables[inst.scope];
      for (auto ident : identifiers)
      {
        stack_offsets[ident] = parent_stack_offset + size;
        size += ident->dataType.size();
      }

      inst.scope->stack_offset = size + parent_stack_offset;

      if (size > 0)
      {
        Line temp("add", {LineArg(*stack_pointer), LineArg(std::to_string(size))});
        AddLine(temp, inst, {});
      }
    }
    else if (inst.instr == Instruction::SCOPE_END)
    {
      auto& identifiers = allocated_variables[inst.scope];
      for (auto ident : identifiers)
      {
        size += ident->dataType.size();
      }
      
      if (size > 0)
      {
        Line temp("sub", {LineArg(*stack_pointer), LineArg(std::to_string(size))});
        PrependLine(temp, inst, {});
      }
    }
    else if (inst.instr == Instruction::RETURN)
    {
      int parent_stack_offset;
      if (inst.scope->scope != SymbolTable::FUNCTION)
        parent_stack_offset = inst.scope->parent->stack_offset;
      else
        parent_stack_offset = 0;

      auto& identifiers = allocated_variables[inst.scope];
      for (auto ident : identifiers)
      {
        size += ident->dataType.size();
      }
      
      if (size > 0)
      {
        Line temp("sub", {LineArg(*stack_pointer), LineArg(std::to_string(size + parent_stack_offset))});
        PrependLine(temp, inst, {});
      }
    }
  }
}

Register::Register(string n, Data d, Register::Rank r, unsigned int b)
{
  loaded = nullptr;
  name = n;
  data = d;
  bytes = b;
  rank = r;
  status = Status::FREE;
  requires_storage = false;
  latest = 0;
  operationStep = 0;
  is_pointer = false;
  pointer = nullptr;
}

Register::Register(const Register& other)
{
  loaded = other.loaded;
  name = other.name;
  data = other.data;
  bytes = other.bytes;
  rank = other.rank;
  status = other.status;
  requires_storage = other.requires_storage;
  latest = other.latest;
  operationStep = other.operationStep;
  is_pointer = other.is_pointer;
  pointer = other.pointer;
}

void Register::load(Result& id, bool isptr, Register* ptr)
{
  loaded = &id;
  if (!id.isConst())
  {
    latest = id.id->latest;
    status = Status::USED;
    is_pointer = isptr;
    pointer = ptr;
  }
  else
  {
    // status = Status::FREE;
  }
}

void Register::flush()
{
  loaded = nullptr;
  latest = 0;
  status = Status::FREE;
  requires_storage = false;
  operationStep = 0;
  is_pointer = false;
  pointer = nullptr;
}

// TODO -- this is annoyingly hardcoded again
LineArg::LineArg(Register& r, bool ptr)
{
  t = TYPE::REGISTER;
  pointer_op = ptr;
  reg = &r;
}
LineArg::LineArg(Result& r, bool ptr)
{
  t = TYPE::RESULT;
  pointer_op = ptr;
  result = &r;
}
LineArg::LineArg(string s, bool ptr)
{
  t = TYPE::STRING;
  pointer_op = ptr;
  str = s;
}
// LineArg::LineArg(LineArg& other)
// {
//   t = other.t;
//   reg = other.reg;
//   result = other.result;
//   str = other.str;
// }
string LineArg::to_string()
{
  string out = "";
  switch (t)
  {
    case TYPE::REGISTER:
      out = reg->name;
      break;
    case TYPE::RESULT:
      out = result->to_string();
      break;
    case TYPE::STRING:
      out = str;
      break;
    default:
      throw 1;
      break;
  }
  if (pointer_op)
    return '[' + out + ']';
  else
    return out;
}

Line::Line(string mnem, vector<LineArg> a)
{
  mnemonic = mnem;
  args = a;
}
Line::Line(string mnem, vector<LineArg> a, string cond)
{
  mnemonic = mnem;
  args = a;
  condition = cond;
}
string Line::to_string()
{
  string out = mnemonic + " ";
  for (int i = 0; i < args.size(); i++)
  {
    out += args[i].to_string();
    if (i < args.size() - 1)
      out += ", ";
  }
  if (condition != "")
    out += " -> " + condition;
  return out;
}

// TODO -- this is implementation-specific, so we need to figure out a
// general solution here
string Line::to_string(unordered_map<Identifier*, int> stack_offsets, string base_reg)
{
  string out = mnemonic + " ";
  for (int i = 0; i < args.size(); i++)
  {
    if (
      args[i].GetType() == LineArg::RESULT &&
      (args[i].result->kind == Result::ID || args[i].result->kind == Result::POINTER) &&
      stack_offsets.count(args[i].result->id) > 0
    )
    {
      // out += '[' + base_reg + ',' + std::to_string(stack_offsets[args[i].result->id]) + ']' + " (" + args[i].result->id->name + ")";
      if (args[i].result->isConst())
        out += '[' + args[i].to_string() + ']';
      else
        out += '[' + base_reg + ',' + std::to_string(stack_offsets[args[i].result->id]) + ']';
    }
    else
    {
      out += args[i].to_string();
    }
    
    if (i < args.size() - 1)
      out += ", ";
  }
  if (condition != "")
    out += " -> " + condition;
  return out;
}

void BaseTarget::TranslateAll()
{
  for (const auto id : comp->globalTable->ordered)
  {
    if (id->type == Identifier::IdType::FUNCTION)
      Translate(*id);
  }
}

void BaseTarget::Translate(Identifier& function)
{
  operationStep = 0;
  FuncTrans temptrans;
  temptrans.id = &function;
  temptrans.language = targetName;
  temptrans.instructions = &function.function.instructions;
  translations.push_back(temptrans);

  for (auto& inst : function.function.instructions)
  {
    (this->*methods[(int) inst.instr])(inst);
  }

  // for (int i = 0; i < function.function.instructions.size(); i++)
  // {
  //   (this->*methods[(int) function.function.instructions[i].instr])(function.function.instructions[i]);
  // }

  // store any remaining values before returning
  current_scope = function.funcTable;
  StoreAll(function, function.function.instructions.back());
  SaveUsedRegisters(function); // save all the registers that were used (prepending code)
  translations.back().AssignArgumentOffsets(function);
  RestoreUsedRegisters(function); // restore all the registers that were used (appending code)
  translations.back().AllocateVariables(&GetStackPointer());
  
  ResetRegisters();
  translations.back().Flatten();
}

void BaseTarget::UpdateRegister(Register& reg)
{
  if (reg.loaded != nullptr && !reg.loaded->isConst())
    reg.latest++;
  reg.operationStep = ++operationStep;
}

void BaseTarget::unsupported(Instruction& inst)
{
  string errmess = inst.name() + " operation is not yet supported for \"" + targetName + "\"";
  comp->addNodeError(inst.ctx, errmess);
}

void BaseTarget::unsupported(string mess)
{
  string errmess = mess + " operation is not yet supported for \"" + targetName + "\"";
  comp->err->AddError(mess, -1, "");
}

int BaseTarget::freeRegisters(Register::Data data, Register::Rank rank)
{
  int free = 0;
  for (int i = 0; i < registers.size(); i++)
  {
    if (registers[i].data == data && registers[i].rank == rank && registers[i].isFree())
      free++;
  }
  return free;
}

Register& BaseTarget::getFree(Register::Data data, Register::Rank rank)
{
  for (int i = 0; i < registers.size(); i++)
  {
    if (registers[i].data == data && registers[i].rank == rank && registers[i].isFree())
      return registers[i];
  }
  throw 1;
}

void BaseTarget::ResetRegisters()
{
  for (auto& reg : registers)
  {
    reg.flush();
  }
}

void BaseTarget::StoreRegister(Register& reg, Instruction& inst)
{
  // Not totally sure how we should deal with this, but for now...
  if ((reg.loaded->id->latest < reg.latest || 
      (reg.loaded->id->dataType.qualifiers & Qualifier::VOLATILE)) && 
      reg.requires_storage
  )
  {
    reg.loaded->id->latest = reg.latest;
    TranslateStore(reg, inst);
  }
  // reg.status = Register::Status::FREE;
  reg.requires_storage = false;
}

void BaseTarget::StoreRegister(Register& reg, Instruction& inst, Identifier& ass)
{
  // Not totally sure how we should deal with this, but for now...
  if (ass.latest < reg.latest || (ass.dataType.qualifiers & Qualifier::VOLATILE))
  {
    ass.latest = reg.latest;
    TranslateStore(reg, inst, ass);
  }
  // reg.status = Register::Status::FREE;
  reg.requires_storage = false;
}

// NOTE -- this is only really for use at the end of the function
void BaseTarget::StoreAll(Identifier& function, Instruction& inst, bool include)
{
  for (auto& reg : registers)
  {
    if ((reg.rank == Register::Rank::GENERAL || include) && reg.requires_storage)
    {
      // we should always store pointers
      if (!reg.is_pointer && reg.loaded != nullptr && reg.loaded->id->dataType.isPointer())
      {
        StoreRegister(reg, inst);
      }
      else
      {
        try{
          function.funcTable->GetLocalSymbol(reg.loaded->id->name);
          // if (reg.loaded->id->dataType.qualifiers & Qualifier::VOLATILE) {
          //   // if it's volatile, store it anyway
          //   StoreRegister(reg, inst);
          // }
          StoreRegister(reg, inst);
        } catch (int e) {
          // bit of a hacky way to determine if it's a global, but it works...
          try {
            function.funcTable->GetSymbol(reg.loaded->id->name);
            StoreRegister(reg, inst);
          } catch (int e) {

          }
        }
      }

      
    }
  }
}

Register::Data BaseTarget::FetchDataType(Result& res)
{
  if (res.type.isPointer())
    return GetPointerType();
  for (auto st : StandardTypes)
  {
    if (!res.isConst() && res.id->dataType == *st)
      return datatypes[st];
    if (res.isConst() && res.type == *st)
      return datatypes[st];
  }
  throw 1;
}

Register::Data BaseTarget::FetchDataType(Identifier& id)
{
  if (id.dataType.isPointer())
    return GetPointerType();
  for (auto st : StandardTypes)
  {
    if (id.dataType == *st)
      return datatypes[st];
  }
  throw 1;
}

Register& BaseTarget::GetLastUsed(Instruction& inst, Register::Data data, Register::Rank rank)
{
  auto lowest = std::numeric_limits<unsigned int>::max();
  auto lowestFree = std::numeric_limits<unsigned int>::max();
  Register* lastToStore = nullptr;
  Register* lastFree = nullptr;
  for (auto& reg : registers)
  {
    if (reg.data == data && reg.rank == rank)
    {
      if (!reg.isFree() && reg.operationStep < lowest)
      {
        lowest = reg.operationStep;
        lastToStore = &reg;
      }
      else if (reg.isFree() && reg.operationStep < lowestFree)
      {
        lowestFree = reg.operationStep;
        lastFree = &reg;
      }
    }
  }
  if (lastFree == nullptr) {
    if (lastToStore == nullptr) throw 1;
    StoreRegister(*lastToStore, inst);
    // TranslateStore(*lastToStore);
    return *lastToStore;
  }
  return *lastFree;
}

Register& BaseTarget::PrepareResult(Result& res, Instruction& inst)
{
  // Check if it's already loaded and not out-of-date
  for (auto& reg : registers)
  {
    if (reg.loaded != nullptr && *reg.loaded == res) 
    {
      if (res.isConst() || reg.latest >= res.id->latest)
      {
        reg.status = Register::Status::USED;
        reg.operationStep = ++operationStep;
        return reg;
      }
    }
  }

  Register* reg;
  Register::Data d = FetchDataType(res);

  reg = &GetLastUsed(inst, d);

  reg->operationStep = ++operationStep;
  reg->load(res);
  reg->status = Register::Status::USED;
  TranslateLoad(*reg, inst, res);
  return *reg;
}

// TODO -- this should look for the register with the _latest_ copy of the
// intended assignee
Register& BaseTarget::PrepareAssign(Identifier* res, Instruction& inst)
{
  // Check if it's already loaded and not out-of-date
  for (auto& reg : registers)
  {
    if (reg.loaded != nullptr && !reg.loaded->isConst() && *reg.loaded->id == res) 
    {
      reg.status = Register::Status::USED;
      reg.operationStep = operationStep++;
      return reg;
    }
  }

  Register* reg;
  Register::Data d = FetchDataType(*res);
  
  reg = &GetLastUsed(inst, d);

  Result& temp = GenerateResult(res);
  reg->load(temp);
  reg->status = Register::Status::USED;
  reg->operationStep = operationStep++;
  return *reg;
}

Register& BaseTarget::CheckLoaded(Identifier* res)
{
  // Check if it's already loaded and not out-of-date
  for (auto& reg : registers)
  {
    if (reg.loaded != nullptr && !reg.loaded->isConst() && *reg.loaded->id == res) 
    {
      reg.operationStep = operationStep++;
      return reg;
    }
  }
  throw 1;
}

Result& BaseTarget::GenerateResult(Identifier* id, bool is_pointer)
{
  Result res;
  if (is_pointer)
    res.setPointer(id);
  else
    res.setValue(id);
  temp_results.push_back(res);
  return temp_results.back();
}

Result& BaseTarget::GenerateResult(Result& res)
{
  temp_results.push_back(res);
  return temp_results.back();
}

// Frees registers after statements
void BaseTarget::TranslateStat(Instruction& inst)
{
  for (auto& reg : registers)
  {
    if (!reg.isFree() && (!reg.requires_storage || reg.loaded->id->name[0] == '$'))
    {
      reg.status = Register::Status::FREE;
    }
  }
}

void BaseTarget::ManageStorage(Register& reg, Instruction& inst)
{
  if (reg.loaded != nullptr && !reg.loaded->isConst())
  {
    if (reg.loaded->id->dataType.qualifiers & Qualifier::VOLATILE)
    {
      TranslateStore(reg, inst);
      reg.requires_storage = false;
    }
    else
    {
      reg.requires_storage = true;
    }
  }
}

string BaseTarget::to_string()
{
  string output = "";
  for (auto& func : translations)
  {
    output += "// \"" + func.id->name + "\" compiled to " + targetName + "\n";
    output += "// arguments:\n";
    for (auto& arg : func.id->members)
    {
      output += "//  " + arg.dataType.to_string() + " " + arg.name + "\n";
    }
    output += func.id->name + ":\n";

    for (auto& line : func.translation)
    {
      output += " " + line.to_string(func.stack_offsets, GetBasePointer().name) + "\n";
    }
      
    output += "\n";
  }
  return output;
}

Register& BaseTarget::GetStackPointer()
{
  for (auto& reg : registers)
  {
    if (reg.rank == Register::Rank::STACK_POINTER)
      return reg;
  }
  throw 1;
}

Register& BaseTarget::GetBasePointer()
{
  for (auto& reg : registers)
  {
    if (reg.rank == Register::Rank::BASE_POINTER)
      return reg;
  }
  throw 1;
}

Register& BaseTarget::GetProgramCounter()
{
  for (auto& reg : registers)
  {
    if (reg.rank == Register::Rank::PROGRAM_COUNTER)
      return reg;
  }
  throw 1;
}

void BaseTarget::AddLine(string mnemonic, Instruction& inst, vector<LineArg> args)
{
  Line l(mnemonic, args);
  vector<Register*> regs;
  for (auto& a : args)
  {
    if (a.t == LineArg::TYPE::REGISTER)
    {
      regs.push_back(a.reg);
    }
  }
  translations.back().AddLine(l, inst, regs);
}

void BaseTarget::AddLine(string mnemonic, Instruction& inst, vector<LineArg> args, string condition)
{
  Line l(mnemonic, args, condition);
  vector<Register*> regs;
  for (auto& a : args)
  {
    if (a.t == LineArg::TYPE::REGISTER)
    {
      regs.push_back(a.reg);
    }
  }
  translations.back().AddLine(l, inst, regs);
}

Register& BaseTarget::GetPointer(Identifier* target, Instruction& inst, Register* pointer)
{

  // start with given pointer
  if (pointer != nullptr)
  {
    if (pointer->is_pointer && pointer->loaded != nullptr && pointer->loaded->id != nullptr && pointer->loaded->id == target)
    {
      return *pointer;
    }
  }

  // Check if pointer is already loaded
  for (auto& reg : registers)
  {
    if (reg.is_pointer && reg.loaded != nullptr && reg.loaded->id != nullptr && reg.loaded->id == target)
    {
      return reg;
    }
  }

  Register* reg;
  Register::Data d = FetchDataType(*target);

  reg = &GetLastUsed(inst, d);

  Result& res = GenerateResult(target, true);

  reg->operationStep = ++operationStep;
  reg->load(res, true);
  reg->status = Register::Status::USED;
  TranslateLoad(*reg, inst, res);
  return *reg;
}