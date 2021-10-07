#include <iostream>
#include <fstream>

#include "symbols.h"


int Reference::GetValue(SymbolTable& symbols)
{
  // for now, it'll just get the address
  if (isAddress) {
    return symbols.GetSymbol(name).address;
  } else {
    throw 1;
  }
}

int Value::GetValue(SymbolTable& symbols)
{
  if (type == NUMBER) {
    return value;
  }
  else 
  {
    return reference.GetValue(symbols);
  }
}

Symbol::Symbol(string n)
{
  name = n;
  isLabel = false;
  address = -1;
  index = 0;
}

Symbol::Symbol(string n, size_t idx)
{
  name = n;
  isLabel = true;
  address = 0;
  index = idx;
}

void GatherAttributes(Attributes& to, Attributes& from)
{
  for (auto& pair : to) {
    if (from.count(pair.first) > 0)
      pair.second = from[pair.first];
  }
}

size_t align(size_t input, size_t alignment) {
  if (input % alignment > 0) {
    return input + alignment - (input % alignment);
  } else {
    return input;
  }
}

void VerifyAttributes(Attributes& check, Error* err, ParseTree* node)
{
  bool section = false;
  for (auto& pair : check) 
  {
    if (SymbolTable::sections.count(pair.first) > 0) {
      if (section == true) {
        string errmess = "data cannot specify more than one memory section";
        err->AddNodeErr(errmess, node);
      }
      section = true;
    }
    else if (Section::data_attributes.count(pair.first) > 0) 
    {
      if (pair.first == "align" && pair.second == 0) 
      {
        string errmess = "data alignment of 0 is invalid";
        err->AddNodeErr(errmess, node);
        pair.second = 1;
      }
    }
    else 
    {
      string errmess = "unknown attribute \"" + pair.first + "\"";
      err->AddNodeErr(errmess, node);
    }
  }
}

void Section::Push(SymbolRange& sr) 
{
  sr.symbol->address = sr.start;
  symbols.push_back(sr);
}

// TODO -- this is a horrible mess and you should feel bad for making it
void Section::AddSymbol(Symbol& sym, Error* err, ParseTree* node)
{
  
  Attributes attr = data_attributes;
  GatherAttributes(attr, sym.attributes);
  SymbolRange sr(&sym);

  if (attr["address"] == -1) 
  {
    if (symbols.size() == 0) 
    {
      sr.start = align(start, attr["align"]);
      sr.end = sr.start + sym.data.size();

      if (sr.end > end) {
        err->AddNodeErr("data \"" + sr.symbol->name + "\" too large for section \"" + name + "\"", node);
      } else {
        Push(sr);
      }
    }
    else
    {
      // TODO -- it's possible that the data could become fragmented if a large, aligned
      // array follows a shorter, specified value
      bool found = false;

      size_t temp_start = align(start, attr["align"]);
      size_t temp_end = temp_start + sym.data.size();
      if (ValidRange(temp_start, temp_end, sym, err, node))
      {
        found = true;
        SymbolRange fsr(&sym, temp_start, temp_end);
        Push(fsr);
      }
      if (!found) {
        for (auto& sr : symbols) {
          temp_start = align(sr.end, attr["align"]);
          temp_end = temp_start + sym.data.size();
          if (ValidRange(temp_start, temp_end, sym, err, node))
          {
            found = true;
            SymbolRange fsr(&sym, temp_start, temp_end);
            Push(fsr);
            break;
          }
        }
      }
      if (!found) {
        string errmess = "section \"" + name + "\" unable to hold requested data";
        err->AddNodeErr(errmess, node);
      }

    }
  }
  else // if the address is specified...
  {
    sr.start = align(attr["address"], attr["align"]);
    sr.end = sr.start + sym.data.size();

    if (ValidRange(sr.start, sr.end, sym, err, node))
    {
      SymbolRange fsr(&sym, sr.start, sr.end);
      Push(sr);
    }
    else
    {
      // TODO -- this should really be a warning that attempts to place it after
      err->AddNodeErr("unable to place " + sym.name + " at given address", node);
    }
  }
}

bool Section::ValidRange(size_t s, size_t e, Symbol& sym, Error* err, ParseTree* ctx)
{
  if (s < start || e > end) 
  {
    string errmess = "requested address range (" + to_string(s) + ".." + to_string(e) +
                     ") not within available range (" + to_string(start) + ".." + to_string(end) +
                     "for section \"" + name + "\"";
    err->AddNodeErr(errmess, ctx);
    return false;
  }
  for (auto& sr : symbols) {
    if ((s >= sr.start && s < sr.end) || (e >= sr.start && e < sr.end))
      return false;
  }
  return true;
}

SymbolRange& Section::CheckAddress(uint32_t addr)
{
  for (auto& sr : symbols) {
    if (addr >= sr.start && addr < sr.end)
      return sr;
  }
  return emptyRange;
}

// TODO -- make word size a property of _value_ (for now we'll always assume 16 bits)
vector<uint8_t>& Section::GetData(SymbolTable& sr)
{
  data.resize(end * 2);
  for (int i = 0; i < end * 2; i++)
    data[i] = 0;

  for (auto& range : symbols)
  {
    for (int i = range.start; i < range.end; i++)
    {
      int d = range.symbol->data[i - range.start].GetValue(sr);
      data[i * 2] = d & 255;
      data[i * 2 + 1] = d >> 8;
    }
  }

  // // TODO -- make this more efficient for large data ranges
  // size_t i = start;
  // SymbolRange* range = nullptr;
  // while (i < end)
  // {
  //   if (range == nullptr)
  //   {
  //     SymbolRange& trange = CheckAddress(i);
  //     if (trange.symbol != nullptr)
  //     {
  //       range = &trange;
  //       int d = trange.symbol->data[i - trange.start].GetValue(sr);
  //       data.push_back(d & 255);
  //       data.push_back(d >> 8);
  //       i++;
  //     }
  //     else
  //     {
  //       data.push_back(0);
  //       data.push_back(0);
  //       i++;
  //     }
  //   }
  //   else
  //   {
  //     if (i >= range->end) {
  //       range = nullptr;
  //     }
  //     else
  //     {
  //       int d = range->symbol->data[i - range->start].GetValue(sr);
  //       data.push_back(d & 255);
  //       data.push_back(d >> 8);
  //       i++;
  //     }
  //   }
  // }
  return data;
}

void SymbolTable::AddSymbol(Symbol& s, Error* err, ParseTree* node)
{
  if (symbols.count(s.name) == 0) 
  {

    string section = "ram";
    for (auto& attr : s.attributes)
    {
      if (sections.count(attr.first) > 0) {
        section = attr.first;
      }
    }

    symbols[s.name] = s;
    ordered.push_back(&symbols[s.name]);

    if (!s.isLabel) {
      sections[section].AddSymbol(symbols[s.name], err, node);
    }

  } 
  else 
  {
    throw 1;
  }
}

Symbol& SymbolTable::GetSymbol(string name)
{
  if (symbols.count(name) == 0) 
  {
    throw 1;
  } 
  else 
  {
    return symbols[name];
  }
}

void SymbolTable::WriteFiles(string prefix)
{
  for (auto& pair : sections) {
    auto& data = pair.second.GetData(*this);
    ofstream outfile(prefix + "_" + pair.first + ".bin", ios::out | ios::binary);
    for (auto value : data) {
      outfile << value;
    }
    outfile.close();
  }
}