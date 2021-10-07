#include "stable.h"

void SymbolTable::AddSymbol(Identifier& id)
{ 
  if (symbols.count(id.name) == 0) 
  {
    id.dataType.sortSpecifiers();
    symbols[id.name] = id;
    ordered.push_back(&symbols[id.name]);
  } 
  else 
  {
    Identifier& other = symbols[id.name];

    // If we get here, then the identifier has
    // at least been declared
    if (id.type == Identifier::IdType::FUNCTION && other.type == Identifier::IdType::FUNCTION)
    {
      if (!other.initialized && id == other)
      {
        symbols[id.name] = id;
        ordered.push_back(&symbols[id.name]);
      }
      else if (!other.initialized)
        throw 2;
      else
        throw 3;
    }
    else
      throw 1;
  }
}

Identifier& SymbolTable::GetLocalSymbol(string name)
{
  if (symbols.count(name) == 0) throw 1;
  return symbols[name];
}

Identifier& SymbolTable::GetSymbol(string name)
{
  // first check local copy
  try {
    return GetLocalSymbol(name);
  } catch (int e) {
    // then check parents up the tree
    if (parent == nullptr)
      throw 1;
    else
      return parent->GetSymbol(name);
  }
}

SymbolTable* SymbolTable::GetScope(Identifier* id)
{
  try
  {
    GetLocalSymbol(id->name);
    return this;
  }
  catch (int e)
  {
    if (parent != nullptr)
    {
      return parent->GetScope(id);
    }
    else
      throw 1;
  }
}

Identifier& SymbolTable::GetLast()
{
  return *ordered.back();
}