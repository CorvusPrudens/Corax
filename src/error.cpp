
#include <iostream>
#include <fstream>
#include <string>

#include "error.h"
#include "colors.h"

using std::string;


string get_line(string filename, int line)
{
  std::ifstream file(filename);
  int i = 1; // lines are not zero-indexed
  char c;
  while (i < line) {
    file.read(&c, 1);
    if (file.eof())
    {
      return "???";
    }
    if (c == '\n') i++;
  }
  string out;
  std::getline(file, out);

  return out + "\n";
}

void Error::AddError(string message, int line, string file, int code, int colstart, int colend, bool fatal)
{
  item e {message, line, file, code, colstart, colend};
  errors.push_back(e);
  if (fatal || errors.size() >= max_errors)
    Report();
}

void Error::AddWarning(string message, int line, string file, int code, int colstart, int colend)
{
  item w {message, line, file, code, colstart, colend};
  warnings.push_back(w);
}

// TODO -- add detection for rules spanning multiple lines!
void Error::AddNodeErr(string message, ParseTree* node, string file, antlr4::CommonTokenStream* toks)
{
  auto start = node->getSourceInterval().a;
  auto end = node->getSourceInterval().b;

  int line = toks->get(start)->getLine();
  int cols = toks->get(start)->getCharPositionInLine();
  int cole = cols + toks->get(end)->getStopIndex() - toks->get(start)->getStartIndex();
  AddError(message, line, file, 1, cols, cole);
}

void Error::AddNodeWarn(string message, ParseTree* node, string file, antlr4::CommonTokenStream* toks)
{
  auto start = node->getSourceInterval().a;
  auto end = node->getSourceInterval().b;
  
  int line = toks->get(start)->getLine();
  int cols = toks->get(start)->getCharPositionInLine();
  int cole = cols + toks->get(end)->getStopIndex() - toks->get(start)->getStartIndex();
  AddWarning(message, line, file, 1, cols, cole);
}

void Error::Report()
{
  int num_errors = errors.size();
  int num_warnings = warnings.size();

  if (num_errors > 0 || num_warnings > 0)
  {
    std::cout << "\n";

    if (num_errors > 0)
    PrintErrors();
  
    if (num_warnings > 0)
      PrintWarnings();
    
    string ess = num_errors == 1 ? "" : "s";
    std::cout << num_errors << Colors::Redl << " error" << ess << Colors::Stop << ", ";
    
    ess = num_warnings == 1 ? "" : "s";
    std::cout << num_warnings << Colors::Bluel << " warning" << ess << Colors::Stop;

    if (num_errors > 0)
    {
      if (errors.size() >= max_errors)
        std::cout << ", (error limit reached) exiting...\n";
      else
        std::cout << ", exiting...\n";
      exit(1);
    }
    std::cout << "\n";
  }
  
}

void Error::PrintErrors()
{
  for (auto& e : errors)
  {
    if (e.file != "-1" && e.line != -1)
    {
      std::cout << Colors::Redl << "Error " << Colors::Stop;
      std::cout << "in file " << Colors::Green << "\"" << e.file << "\" " << Colors::Stop;
      std::cout << "at line " << e.line << ":\n";

      string line = get_line(e.file, e.line);
      std::cout << Colors::Gray << line << Colors::Stop;
      if (e.colstart != -1)
      {
        string underline = "";
        for (int i = 0; i < e.colstart; i++)
        {
          underline += " ";
        }
        underline += Colors::Redl;
        for (int i = e.colstart; i < e.colend + 1; i++)
        {
          underline += "^";
        }
        std::cout << underline << Colors::Stop << "\n";
      }
      std::cout << Colors::Gray << "  -> " << Colors::Stop << e.message << "\n\n";
    }
    else
    {
      std::cout << Colors::Red << "Error: \n" << Colors::Stop;
      std::cout << Colors::Gray << "  -> " << Colors::Stop << e.message << "\n\n";
    }
  }
}

void Error::PrintWarnings()
{
  for (auto& w : warnings)
  {
    if (w.file != "-1" && w.line != -1)
    {
      std::cout << Colors::Bluel << "Warning " << Colors::Stop;
      std::cout << "in file " << Colors::Green << "\"" << w.file << "\" " << Colors::Stop;
      std::cout << "at line " << w.line << ":\n";

      string line = get_line(w.file, w.line);
      std::cout << Colors::Gray << line << Colors::Stop;
      if (w.colstart != -1)
      {
        string underline = "";
        for (int i = 0; i < w.colstart; i++)
        {
          underline += " ";
        }
        underline += Colors::Bluel;
        for (int i = w.colstart; i < w.colend + 1; i++)
        {
          underline += "^";
        }
        std::cout << underline << Colors::Stop << "\n";
      }
      std::cout << Colors::Gray << "  -> " << Colors::Stop << w.message << "\n\n";
    }
    else
    {
      std::cout << Colors::Redl << "Error: \n" << Colors::Stop;
      std::cout << Colors::Green << "  -> " << Colors::Stop << w.message << "\n\n";
    }
  }
}