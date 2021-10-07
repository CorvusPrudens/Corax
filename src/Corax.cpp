
#include <iostream>
#include <sstream>
#include <filesystem>

#include "CLI.hpp"
#include "precompiler.h"
#include "compiler.h"
#include "assembler.h"
#include "error.h"

#include "corvassembly.h"

using std::filesystem::path;

int main(int argc, const char* argv[])
{

  Error err;

  CLI::App app{"Corax -- a simple, retargetable C compiler"};

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


  Assembler enca(assembly, err);

  comp.Complete();

  for (auto& inst : enca.instructions) {
    cout << inst.mnemonic << "\n";
  }
  for (auto symbol : enca.symbols.ordered) {
    cout << symbol->name << " " << symbol->address << "\n";
  }

  cout << "\n";
  bool debug = true;
  bool no_output = false;
  string outname = "bin";
  
  if (debug) 
  {
    for (auto& inst : enca.instructions) {
      cout << formatInstruction(inst.machine.bytes) + "\n";
    }
  }

  if (!no_output) 
  {
    ofstream outfile(outname + ".bin", ios::out | ios::binary);
    for (auto item : enca.machine_code) {
      outfile << item;
    }
    outfile.close();
    enca.symbols.WriteFiles(outname);
  }

  return 0;
}