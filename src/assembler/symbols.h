
#ifndef ENCA_SYMBOLS_H
#define ENCA_SYMBOLS_H

#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include "error.h"
#include "antlr4-runtime.h"

using namespace std;
using antlr4::tree::ParseTree;

typedef unordered_map<string, int> Attributes;
class SymbolTable; // forward decl

struct Reference {

  Reference() {}
  Reference(string n) { name = n; isAddress = false; }

  int GetValue(SymbolTable& symbols);

  string name;
  bool isAddress;
};

struct Value {

  enum Type {
    NUMBER = 0,
    REFERENCE,
  };

  Value() {}
  Value(int v) { value = v; type = NUMBER; }
  Value(Reference r) { reference = r; type = REFERENCE; }

  int GetValue(SymbolTable& symbols);

  Type type;
  int value;
  Reference reference;
};

class Symbol {

  public:
    Symbol() {}
    Symbol(string n);
    Symbol(string n, size_t idx);
    ~Symbol() {}


    string name;
    int address;
    Attributes attributes;
    vector<Value> data;
    size_t index;
    bool isLabel;
};

struct SymbolRange {
  SymbolRange(Symbol* sym) { symbol = sym; }
  SymbolRange(Symbol* sym, size_t s, size_t e) {
    symbol = sym;
    start = s;
    end = e;
  }
  Symbol* symbol;
  size_t start;
  size_t end;
};

class Section {

  public:

    Section() {}
    Section(string n, size_t s, size_t e) { 
      name = n; 
      start = s;
      end = e;
    }
    ~Section() {}

    void AddSymbol(Symbol& sym, Error* err, ParseTree* node);
    bool ValidRange(size_t s, size_t e, Symbol& sym, Error* err, ParseTree* node);
    vector<uint8_t>& GetData(SymbolTable& sr);
    SymbolRange& CheckAddress(uint32_t addr);
    void Push(SymbolRange& sr);

    inline static Attributes data_attributes = {
      {"ram", -1}, // section
      {"rom", -1}, // section
      {"flash", -1}, // section
      {"address", -1},
      {"align", 1},
    };

    string name;
    list<SymbolRange> symbols;
    inline static SymbolRange emptyRange{nullptr, 0, 0};
    vector<uint8_t> data;
    size_t start;
    size_t end;

};

// No scopes to worry about for this one
class SymbolTable {

  public:
    SymbolTable() {}
    ~SymbolTable() {}

    void AddSymbol(Symbol& s, Error* err, ParseTree* node);
    Symbol& GetSymbol(string name);

    // I'm sure we'll add options later
    void WriteFiles(string prefix);

    inline static unordered_map<string, Section> sections = {
      {"ram", Section("ram", 65, 1792)}, // 256 words reserved for stack
      {"rom", Section("rom", 0, 0x10000)},
      {"flash", Section("flash", 0, 0x800000)},
    };

    unordered_map<string, Symbol> symbols;
    vector<Symbol*> ordered;
};

#endif