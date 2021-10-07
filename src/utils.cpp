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