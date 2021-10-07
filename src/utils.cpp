#include "utils.h"

string file_string(string filename)
{
  std::ifstream stream(filename);
  std::stringstream buffer;
  buffer << stream.rdbuf();
  return buffer.str();
}

void Graph::Addf(string func)
{ 
  if (GetIdx(func) == -1)
    functions.push_back(func); 
  currentFunc = func;
}

void Graph::Addc(string call)
{
  Call c;
  c.called = GetIdx(call);
  c.caller = GetIdx(currentFunc);
  calls.push_back(c);
}

void Graph::Print()
{
  cout << "digraph G {\n";
  cout << "ranksep=.25;\n";
  cout << "edge [arrowsize=.5]";
  cout << "node [shape=circle, fontname=\"ArialNarrow\",";
  cout << "fontsize=12, fixedsize=true, height=.6];\n";

  for (auto f : functions)
  {
    cout << f << "; ";
  }
  cout << "\n";

  for (auto c : calls)
  {
    cout << GetName(c.caller) << " -> ";
    cout << GetName(c.called) << ";\n"; 
  }
  cout << "}\n";
}

int Graph::GetIdx(string name)
{
  for (int i = 0; i < functions.size(); i++)
  {
    if (name == functions[i])
      return i;
  }
  return -1;
}

string Graph::GetName(int idx)
{
  if (idx > -1 && idx < functions.size())
    return functions[idx];
  else
    return "<invalid function>";
}

void LineData::add(int line_in, int line_out, string file)
{
  int file_ref;
  if (f2idx.count(file) == 0)
  {
    file_ref = files.size();
    f2idx[file] = file_ref;
    files.push_back(file);
  }
  else
  {
    file_ref = f2idx[file];
  }

  LineReference data{line_in, line_out, file_ref};
  lines.push_back(data);
}

string ProcessedCode::getMacro(string name)
{
  if (macros.count(name) == 0)
  {
    throw 1;
  }
  return macros[name];
}

void ProcessedCode::addMacro(string name, string text)
{
  if (macros.count(name) > 0)
  {
    throw 2;
  }
  macros[name] = text;
}

void ProcessedCode::removeMacro(string name)
{
  // actually, this should just silently pass
  if (macros.count(name) == 0)
  {
    return;
  }
  macros.erase(name);
}

void ProcessedCode::addLine(int line_in, string file, string line)
{
  code += line;
  lines.add(line_in, current_line++, file);
}

string hex(int value) {
  std::stringstream stream;
  stream << std::hex << value;
  return stream.str();
}

#include "instruction.h"
#include "operand.h"

string getMnemonic(int code) {
  for (auto& pair : assembler::Instruction::mnemonics) {
    if (code == pair.second.opcode) {
      return pair.first;
    }
    else if (code == pair.second.conditional) {
      return pair.first + " (conditional)";
    }
  }
  return "<invalid opcode>";
}

string getRegister(int reg) {
  if (reg < (int) RegisterOp::Index::REG_COUNT) {
    return RegisterOp::Names[reg];
  }
  return "<invalid register>";
}

string formatInstruction(std::vector<uint8_t> machine, bool annotated) 
{
  int c[32];
  
  for (int i = 0; i < machine.size(); i++) {
    for (int j = 0; j < 8; j++) {
      c[j + i * 8] = (machine[i] & (1 << j)) > 0;
    }
  }

  string out_1a = "";
  string out_1b = "";
  string out_2a = "";
  string out_2b = "";
  int pos = 8 * machine.size() - 1;
  
  if (pos > 16) {
    out_2a = "-----------------\n";
    for ( ; pos > 23; pos--) {
      out_2a += std::to_string(c[pos]);
      out_2b += "i";
    }
    out_2a += " ";
    out_2b += " ";
    for ( ; pos > 15; pos--) {
      out_2a += std::to_string(c[pos]);
      out_2b += "i";
    }
    int value = machine[2] + (machine[3] << 8);
    out_2a += " (0x" + hex(value) + ", " + std::to_string(value) + ")\n";
    out_2b += "\n";
  }

  for ( ; pos > 12; pos--) {
    out_1a += std::to_string(c[pos]);
    out_1b += "y";
  }
  out_1a += " ";
  out_1b += " ";
  for ( ; pos > 9; pos--) {
    out_1a += std::to_string(c[pos]);
    out_1b += "b";
  }
  out_1a += " ";
  out_1b += " ";
  for ( ; pos > 6; pos--) {
    out_1a += std::to_string(c[pos]);
    out_1b += "a";
  }
  out_1a += " ";
  out_1b += " ";
  for ( ; pos > 4; pos--) {
    out_1a += std::to_string(c[pos]);
    out_1b += "v";
  }
  out_1a += " ";
  out_1b += " ";
  for ( ; pos > -1; pos--) {
    out_1a += std::to_string(c[pos]);
    out_1b += "o";
  }

  out_1a += " (" + getMnemonic(machine[0] & 0b11111);
  out_1a += " | a: " + getRegister((machine[0] >> 7) + ((machine[1] << 1) & 0b111));
  out_1a += ", b: " + getRegister((machine[1] >> 2) & 0b111);
  out_1a += ", y: " + getRegister((machine[1] >> 5) & 0b111);
  out_1a += ")\n";
  out_1b += "\n";

  return out_1a + out_1b + out_2a + out_2b;
}