
#include <iostream>
#include <sstream>
#include <filesystem>

#include "CLI.hpp"
#include "precompiler.h"
#include "compiler.h"
#include "error.h"

#include "corvassembly.h"

using std::filesystem::path;

int main(int argc, const char* argv[])
{

  Error err;

  CLI::App app{"Compiler for the Corvus flavor of C"};

  std::string filename;
  app.add_option("file", filename, "File name")
    ->required()
    ->check(CLI::ExistingFile);
  bool graph = false;
  app.add_flag("--graph", graph, "Enable function call graphing");

  CLI11_PARSE(app, argc, argv);

  path infile = filename;

  ProcessedCode code;

  Precompiler pre(&code, infile, &err);

  pre.Process();

  int prevnew = 0;
  int curLine = 0;
  // if (!graph)
  // {
  //   std::cout << curLine + 1 << ", " << code.lines.getLine(curLine) << ": ";
  //   curLine++;
  //   for (int i = 0; i < code.code.length(); i++)
  //   {
  //     if (code.code[i] == '\n')
  //     {
  //       std::cout << code.code.substr(prevnew, i + 1 - prevnew);
  //       prevnew = i + 1;
  //       std::cout << curLine + 1 << ", " << code.lines.getLine(curLine) << ": ";
  //       curLine++;
  //     }
  //   }
    
  // }
  // std::cout << code.code;

  // comp.EnableGraph(graph);
  // comp.Process(&code, &err);

  Compiler comp(&code, &err);

  CorvassemblyTarget cor(&comp);
  cor.TranslateAll();

  string assembly = cor.to_string();
  std::cout << assembly;



  comp.Complete();

  return 0;
}