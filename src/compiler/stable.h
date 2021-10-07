
#include <vector>
#include <list>
#include <unordered_map>
#include <memory>
#include <string>

#include "identifier.h"

using std::string;
using std::vector;
using std::list;
using std::unique_ptr;
using std::unordered_map;

struct SymbolTable {

  enum Scope {
    GLOBAL = 0,
    FUNCTION,
    LOCAL,
  };

  SymbolTable(SymbolTable* p, Scope s)
  {
    parent = p;
    scope = s;
    temp_vars = 0;
    total_temps = 0;
    local_stored_bytes = 0;
    stack_offset = 0;
  }

  // TODO -- this needs to check if the symbol already exists. 
  // If it's a defined function or a declared variable, then
  // add an error or throw an exception or something
  void AddSymbol(Identifier& id);

  /** Find a local symbol with the identifier `name`
   * 
   */
  Identifier& GetLocalSymbol(string name);

  /** Search the symbol table tree for a symbol with the identifier `name`
   * 
   */
  Identifier& GetSymbol(string name);

  /** Gets the most recently added symbol
   * 
   */
  Identifier& GetLast();

  /** Get the symbol table that owns the given identifier
   * 
   */
  SymbolTable* GetScope(Identifier* id);

  SymbolTable* parent;
  // Doesn't need to know child, since we only
  // need to walk up the tree
  // vector<SymbolTable> children;

  Scope scope;
  // string identifier;
  // vector<string> symbols;
  unordered_map<string, Identifier> symbols;
  vector<Identifier*> ordered;
  vector<Instruction> postExpr;
  list<SymbolTable> children;

  unsigned int temp_vars;
  unsigned int total_temps;

  unsigned int local_stored_bytes;
  int stack_offset;

};