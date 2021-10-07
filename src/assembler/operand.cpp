#include <algorithm>
#include "operand.h"

void Operand::setRelative(int16_t offset)
{
  relativeOffset = offset;
  isRelative = true;
}

RegisterOp::RegisterOp(string name)
{
  string* idx = std::find(Names, Names + REG_COUNT, name);
  if (idx == Names + REG_COUNT) throw name;

  index = (Index) (idx - Names);
}

ConditionOp::ConditionOp(string c)
{
  string* idx = std::find(Names, Names + COND_COUNT, c);
  if (idx == Names + COND_COUNT) throw c;

  condition = (Cond) (idx - Names);
}

uint16_t ConditionOp::getValue() 
{ 
  return 1 << ((uint16_t) condition); 
}

uint16_t VariableOp::getValue()
{
  return table->GetSymbol(name).address;
}

// Oh frick yeah this is so tasty
Operand::Class VariableOp::getClass()
{ 
  if (isAddress)
    return isRelative ? NUMBER_REL : NUMBER; 
  else if (table->GetSymbol(name).isLabel)
    return isRelative ? NUMBER_REL : NUMBER; 
  else
    return isRelative ? NAME_REL : NAME; 
}