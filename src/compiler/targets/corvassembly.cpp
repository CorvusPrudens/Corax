
#include "corvassembly.h"

CorvassemblyTarget::CorvassemblyTarget(Compiler* c)
{
  comp = c;
  targetName = "Corvassembly";
  conditionalLabels = 0;
  operationStep = 0;

  registers = {
    Register("r0", Register::Data::INTEGER, Register::Rank::GENERAL, 2),
    Register("r1", Register::Data::INTEGER, Register::Rank::GENERAL, 2),
    Register("r2", Register::Data::INTEGER, Register::Rank::GENERAL, 2),
    Register("r3", Register::Data::INTEGER, Register::Rank::GENERAL, 2),
    Register("r4", Register::Data::INTEGER, Register::Rank::GENERAL, 2),

    Register("sp", Register::Data::INTEGER, Register::Rank::STACK_POINTER, 2),
    Register("bp", Register::Data::INTEGER, Register::Rank::BASE_POINTER, 2),
    Register("pc", Register::Data::INTEGER, Register::Rank::PROGRAM_COUNTER, 2),
  };

  returnVal = &registers[0];
}

void CorvassemblyTarget::StandardInstruction(Instruction& inst, string mnemonic)
{
  if (inst.operand1.isConst())
  {
    Register& op2 = PrepareResult(inst.operand2, inst);
    Register& ass = PrepareAssign(*inst.assignment, inst);

    if (&ass == &op2)
      AddLine(mnemonic, inst, {LineArg(op2), LineArg(inst.operand1)});
    else
      AddLine(mnemonic, inst, {LineArg(op2), LineArg(inst.operand1), LineArg(ass)});

    UpdateRegister(ass);
    ManageStorage(ass, inst);
  }
  else if (inst.operand2.isConst())
  {
    Register& op1 = PrepareResult(inst.operand1, inst);
    Register& ass = PrepareAssign(*inst.assignment, inst);

    if (&ass == &op1)
      AddLine(mnemonic, inst, {LineArg(op1), LineArg(inst.operand2)});
      // AddLine(mnemonic + " " + LineArg(op1) + ", " + LineArg(inst.operand2));
    else
      AddLine(mnemonic, inst, {LineArg(op1), LineArg(inst.operand2), LineArg(ass)});
      // AddLine(mnemonic + " " + LineArg(op1) + ", " + LineArg(inst.operand2) + ", " + LineArg(ass));

    UpdateRegister(ass);
    ManageStorage(ass, inst);
  }
  else // they can't both be const! (cause that would just be made an assignment)
  {
    // TODO -- in reality, these should all have a first pass search, and if
    // it's not already loaded somewhere, then after all the other operands
    // have had their search, we'll have a second round of searching/loading!!!
    Register& op1 = PrepareResult(inst.operand1, inst);

    // TODO -- this should be checked -- if it's in a register, then use that,
    // otherwise it should just come from the variable (e.g. add r0, variable)
    // Register& op2 = PrepareResult(inst.operand2, inst);

    Register& ass = PrepareAssign(*inst.assignment, inst);

    
    // if (&ass == &op1)
    //   AddLine(mnemonic, inst, {LineArg(op1), LineArg(op2)});
    // // TODO -- this doesn't work for subtraction or division operations, 
    // // so it has to be commented out
    // // else if (&ass == &op2)
    //   // AddLine(mnemonic, inst, {LineArg(op2), LineArg(op1)});
    // else
    //   AddLine(mnemonic, inst, {LineArg(op1), LineArg(op2), LineArg(ass)});
    //   // AddLine(mnemonic + " " + LineArg(op1) + ", " + LineArg(op1) + ", " + LineArg(ass));

    try 
    {
      Register& op2 = CheckLoaded(*inst.operand2.id);
      AddLine(mnemonic, inst, {LineArg(op1), LineArg(op2), LineArg(ass)});
    } 
    catch (int e)
    {
      AddLine(mnemonic, inst, {LineArg(op1), LineArg(inst.operand2), LineArg(ass)});
    }

    UpdateRegister(ass);
    ManageStorage(ass, inst);
  }
}

void CorvassemblyTarget::TranslateAdd(Instruction& inst)
{
  StandardInstruction(inst, "add");
}
void CorvassemblyTarget::TranslateSub(Instruction& inst)
{
  StandardInstruction(inst, "sub");
}
void CorvassemblyTarget::TranslateMult(Instruction& inst)
{
  StandardInstruction(inst, "mult");
}
void CorvassemblyTarget::TranslateDiv(Instruction& inst)
{
  StandardInstruction(inst, "div");
}
void CorvassemblyTarget::TranslateMod(Instruction& inst)
{
  StandardInstruction(inst, "mod");
}

void CorvassemblyTarget::TranslateShiftl(Instruction& inst)
{
  StandardInstruction(inst, "lsl");
}
void CorvassemblyTarget::TranslateShiftr(Instruction& inst)
{
  StandardInstruction(inst, "lsr");
}
void CorvassemblyTarget::TranslateBitAnd(Instruction& inst)
{
  StandardInstruction(inst, "and");
}
void CorvassemblyTarget::TranslateBitXor(Instruction& inst)
{
  StandardInstruction(inst, "xor");
}
void CorvassemblyTarget::TranslateBitOr(Instruction& inst)
{
  StandardInstruction(inst, "or");
}

void CorvassemblyTarget::TranslateAnd(Instruction& inst)
{
  unsupported(inst);
}
void CorvassemblyTarget::TranslateOr(Instruction& inst)
{
  unsupported(inst);
}

#define AFFIRMATIVE(condition) \
AddLine("joc " condition ", " + successLabel); \
AddLine("jmp " + failLabel); \
AddLine(successLabel + ":"); \
AddLine("ldr " + LineArg(ass) + ", 1"); \
AddLine("jmp " + endLabel); \
AddLine(failLabel + ":"); \
AddLine("ldr " + LineArg(ass) + ", 0"); \
AddLine(endLabel + ":");

// TODO -- just do cps for this!
void CorvassemblyTarget::TranslateCmp(Instruction& inst)
{
  // // if (inst.operand1.isConst()) {
  // //   Register& op1 = PrepareResult(inst.operand1);
  // //   AddLine("cmp " + LineArg(op1) + ", " + LineArg(inst.operand2));

  // // } 
  // if (inst.operand2.isConst()) {
  //   Register& op1 = PrepareResult(inst.operand1);
  //   // AddLine("cmp " + LineArg(op1) + ", " + LineArg(inst.operand2));
  //   AddLine({"cmp", LineArg(op1), LineArg(inst.operand2)});
  // } else {
  //   Register& op1 = PrepareResult(inst.operand1);
  //   Register& op2 = PrepareResult(inst.operand2);
  //   // AddLine("cmp " + LineArg(op1) + ", " + LineArg(op1));
  //   AddLine({"cmp", LineArg(op1), LineArg(op1)});
  // }

  // Register& ass = PrepareAssign(*inst.assignment);

  // Result c1;
  // c1.setValue(1);
  // Result& const1 = GenerateResult(c1);
  // c1.setValue(0);
  // Result& const0 = GenerateResult(c1);

  // string successLabel = "$succ_" + std::to_string(conditionalLabels++);
  // string failLabel = "$fail_" + std::to_string(conditionalLabels++);
  // string endLabel = "$end_" + std::to_string(conditionalLabels++);

  // switch (inst.condition) {
  //   case Instruction::EQUAL:
  //     {
  //       AFFIRMATIVE("equal")
  //     }
  //     break;
  //   case Instruction::NOT_EQUAL:
  //     {
  //       AddLine("joc equal, " + failLabel);
  //       AddLine("ldr " + LineArg(ass) + ", 1");  
  //       AddLine("jmp " + endLabel); 
  //       AddLine(failLabel + ":"); 
  //       AddLine("ldr " + LineArg(ass) + ", 0"); 
  //       AddLine(endLabel + ":");
  //     }
  //     break;
  //   case Instruction::GREATER:
  //     {
  //       AFFIRMATIVE("greater")
  //     }
  //     break;
  //   case Instruction::LESS:
  //     {
  //       AFFIRMATIVE("less")
  //     }
  //     break;
  //   case Instruction::GREATER_EQUAL:
  //     {
  //       unsupported(inst);
  //     }
  //     break;
  //   case Instruction::LESS_EQUAL:
  //     {
  //       unsupported(inst);
  //     }
  //     break;
  //   default:
  //     throw 1;
  // }
  // UpdateRegister(ass);
}
void CorvassemblyTarget::TranslateDeref(Instruction& inst)
{
  unsupported(inst);
}
void CorvassemblyTarget::TranslateNot(Instruction& inst)
{
  Register& op1 = PrepareResult(inst.operand1, inst);
  Register& ass = PrepareAssign(*inst.assignment, inst);

  if (&ass == &op1)
    AddLine("not", inst, {LineArg(op1)});
  else
    AddLine("not", inst, {LineArg(op1), LineArg(*ass.loaded)});

  UpdateRegister(ass);
  ManageStorage(ass, inst);
}
void CorvassemblyTarget::TranslateNegate(Instruction& inst)
{
  unsupported(inst);
}
void CorvassemblyTarget::TranslateNegative(Instruction& inst)
{
  unsupported(inst);
}
void CorvassemblyTarget::TranslateConvert(Instruction& inst)
{
  unsupported(inst);
}
// This could probably be cleverly optimized!
void CorvassemblyTarget::TranslateAssign(Instruction& inst)
{
    Register& op2 = PrepareResult(inst.operand1, inst);

    try {
      Register& ass = CheckLoaded(*inst.assignment);
      // AddLine("add " + LineArg(op1) + ", 0, " + LineArg(ass));
      AddLine("add", inst, {LineArg(op2), LineArg("0"), LineArg(ass)});
      ManageStorage(ass, inst);
    } catch (int e) {
      // for now, since a register can only hold one pointer,
      // the loaded value will change to the assignee
      Result& temp = GenerateResult(*inst.assignment);
      // this would be ideal, but the register needs a pointer to a result!!
      op2.load(temp);
      UpdateRegister(op2);
      ManageStorage(op2, inst);
      // StoreRegister(op2, *inst.assignment);
    }
}

void CorvassemblyTarget::TranslateCall(Instruction& inst)
{
  // the A register is the only one we'll save before the jump
  if (returnVal->loaded != nullptr && !returnVal->loaded->isConst())
    StoreRegister(*returnVal, inst);
  for (int i = inst.args.size() - 1; i > -1; i--)
  {
    Register& pr = PrepareResult(inst.args[i], inst);
    AddLine("push", inst, {pr});
  }
  // AddLine("push pc, 4");
  AddLine("push", inst, {LineArg(GetProgramCounter()), LineArg("4")});
  Result& res = GenerateResult(*inst.function);
  AddLine("jmp", inst, {LineArg(res)});

  size_t sp_offset = 0;
  for (auto& arg : inst.args)
    sp_offset += arg.getSize() / 2; // corvassembly is a strictly 16-bit-word language
  AddLine("add", inst, {LineArg(GetStackPointer()), LineArg(std::to_string(sp_offset + 1))});
  // Register& reg = PrepareAssign(inst.assignment);
  try {
    Register& ass = CheckLoaded(*inst.assignment);
    // AddLine("add " + returnVal->name + ", 0, " + LineArg(ass));
    AddLine("add", inst, {LineArg(*returnVal), LineArg("0"), LineArg(ass)});
    UpdateRegister(ass);
    ManageStorage(ass, inst);
  } catch (int e) {
    Result& res = GenerateResult(*inst.assignment);
    returnVal->load(res);
    UpdateRegister(*returnVal);
    ManageStorage(*returnVal, inst);
  }
}

// #define FUNC_END \
// AddLine({"<restore used>"}); \
// AddLine("add bp, 0, " + GetStackPointer().name); \
// AddLine("pop " + GetBasePointer().name); \
// AddLine("jmp [" + GetStackPointer().name + "]");

void CorvassemblyTarget::TranslateReturnPrep(Instruction& inst)
{
  if (!inst.operand1.isConst() && !(inst.operand1.type == void_))
  {
    Register& ret = PrepareResult(inst.operand1, inst);
    if (&ret != returnVal) {
      auto scope = current_scope->GetScope(returnVal->loaded->id);
      if (scope->scope == SymbolTable::GLOBAL)
        StoreRegister(*returnVal, inst);
      AddLine("add", inst, {LineArg(ret), LineArg("0"), LineArg(*returnVal)});
      returnVal->load(inst.operand1);
    }
  }
}

// TODO -- this could be optimized better!!
void CorvassemblyTarget::TranslateReturn(Instruction& inst)
{
  AddLine("add", inst, {LineArg(GetBasePointer()), LineArg("0"), LineArg(GetStackPointer())});
  AddLine("pop", inst, {LineArg(GetBasePointer())});
  AddLine("jmp", inst, {LineArg("[" + GetStackPointer().name + "]")});
}

void CorvassemblyTarget::TranslateSetup(Instruction& inst)
{
  // AddLine("", inst, {LineArg("<save used>")});
}

void CorvassemblyTarget::TranslateIf(Instruction& inst)
{
  
}
void CorvassemblyTarget::TranslateLabel(Instruction& inst)
{
  AddLine(inst.label1->name + ":", inst, {});
}
void CorvassemblyTarget::TranslateConditional(Instruction& inst)
{
  Register& op1 = PrepareResult(inst.operand1, inst);
  Register& op2 = PrepareResult(inst.operand2, inst);
  AddLine("cmp", inst, {LineArg(op1), LineArg(op1)});
  switch (inst.condition)
  {
    default:
    case Instruction::GREATER:
    {
      Result& res1 = GenerateResult(*inst.label1);
      Result& res2 = GenerateResult(*inst.label2);
      AddLine("joc", inst, {LineArg("greater"), LineArg(res1)});
      AddLine("jmp", inst, {LineArg(res2)});
    }
    break;
  }
}

// will only work if it's been loaded _and_ has a non-const loaded value
void CorvassemblyTarget::TranslateStore(Register& reg, Instruction& inst)
{
  if (reg.loaded->isConst())
  {
    throw 1;
  }
  else
  {
    bool allocated = false;
    auto target_scope = current_scope->GetScope(reg.loaded->id);

    for (auto var : translations.back().allocated_variables[target_scope])
    {
      if (var == reg.loaded->id)
      {
        allocated = true;
        break;
      }
    }

    // SUPER UGGO EW (seeing if variable is an arg)
    for (auto& arg : translations.back().id->members)
    {
      try 
      {
        translations.back().id->funcTable->GetLocalSymbol(arg.name);
        allocated = true;
        break;
      }
      catch (int e) {}
    }

    if (!allocated)
      translations.back().allocated_variables[target_scope].push_back(reg.loaded->id);

    AddLine("str", inst, {LineArg(reg), LineArg(*reg.loaded)});
  }
}
void CorvassemblyTarget::TranslateStore(Register& reg, Instruction& inst, Identifier& id)
{
  // string line = "";
  // line += "str " + reg.name + ", " + id.name;
  // AddLine(line);
  Result& res = GenerateResult(id);
  AddLine("Str", inst, {LineArg(reg), LineArg(res)});
}
void CorvassemblyTarget::TranslateLoad(Register& reg, Instruction& inst, Result& res)
{
  // string line = "ldr " + reg.name + ", " + res.to_string();
  // AddLine(line);
  AddLine("ldr", inst, {LineArg(reg), LineArg(res)});
}

// TODO -- variable management needs to be carefully done!
// The _true_ order of temp variables will be determined in the
// last step, since some may only exist in the registers
// We'll need to know if a declared variable is local or not
// in the last step, though, since it'll be added to the stack! 
void CorvassemblyTarget::SaveUsedRegisters(Identifier& function)
{
  auto translation = &translations.back();
  Instruction* setup = translation->GetSetupInstruction();
  // If the function returns nothing, then the r0 register needs to be preserved
  int start_idx = function.dataType == void_ ? 0 : 1;
  for (int i = start_idx; i < registers.size(); i++)
  {
    if (translation->used_registers.count(&registers[i]) && registers[i].rank == Register::GENERAL)
    {
      translation->AddLine(Line("push", {LineArg(registers[i])}), *setup, {});
    }
  }
}

void CorvassemblyTarget::RestoreUsedRegisters(Identifier& function)
{
  auto& translation = translations.back();
  for (auto& inst : *translation.instructions)
  {
    if (inst.instr == Instruction::RETURN)
    {
      int start_idx = function.dataType == void_ ? 0 : 1;
      for (int i = start_idx; i < registers.size(); i++)
      {
        if (translation.used_registers.count(&registers[i]) && registers[i].rank == Register::GENERAL)
        {
          translation.PrependLine(Line("pop", {LineArg(registers[i])}), inst, {});
        }
      }
    }
  }
}

void CorvassemblyTarget::TranslateScopeBegin(Instruction& inst)
{
  current_scope = inst.scope;
}

void CorvassemblyTarget::TranslateScopeEnd(Instruction& inst)
{
  for (auto& reg : registers)
  {
    try {
      if (reg.loaded != nullptr && reg.loaded->kind == Result::Kind::ID)
      {
        current_scope->GetLocalSymbol(reg.loaded->id->name);
        // reg.requires_storage = false;
        reg.flush(); // maybe this should be done?
      }
      
    } catch (int e) {}
  }
  current_scope = inst.scope->parent;
}

void CorvassemblyTarget::TranslateDeclare(Instruction& inst)
{
  // SymbolTable* scope = &current_scope->GetSymbolScope(inst.label1);
  // If a symbol is declared, the scope will of course always be the current one
  auto& translation = translations.back();
  translation.allocated_variables[current_scope].push_back(inst.label1);
}