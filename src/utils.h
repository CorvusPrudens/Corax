
#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include <list>
#include <vector>
#include <unordered_map>

using std::string;
using std::cout;

string file_string(string filename);

class Graph {
  public:
    Graph() {}
    ~Graph() {}

    struct Call {
      int caller;
      int called;
    };

    void Addf(string func);
    void Addc(string call);
    void Print();

  private:
    int GetIdx(string name);
    string GetName(int idx);
    std::vector<string> functions;
    std::vector<Call> calls;
    string currentFunc;
};

class Macro {

  public:
    Macro() {}
    ~Macro() {}

  private:
    string text;
};

// This will permit us to maintain a reference
// to the original file's line in the output
class LineData {

  public:

    struct LineReference {
      int line_in;
      int line_out;
      int file_in;
    };

    void add(int line_in, int line_out, string file);
    int getLine(int line_out) { return lines[line_out].line_in; }
    string getFile(int line_out) { return files[lines[line_out].file_in]; }
    int length() { return lines.size(); }

  private:
    std::unordered_map<string, int> f2idx;
    std::vector<string> files;
    std::vector<LineReference> lines;
};

// TODO -- add something to allow us to associate
// ranges of code that were previously macros so
// error reporting can be a bit more helpful
struct ProcessedCode {
  int current_line = 0;
  LineData lines;
  std::unordered_map<string, string> macros;
  string code = "";

  string getMacro(string name);
  void addMacro(string name, string text);
  void removeMacro(string name);
  void addLine(int line_in, string file, string line);
};

// This should actually be unnecessary assuming 
// the parsing does its job (right??)
// // Is this even valid? XD
// template<typename T>
// bool MatchingList(T list1, T list2, int size)
// {
//   int matchedPair[size];
//   for (int i = 0; i < size; i++)
//   {
//     auto item = list1[i];
//     for (int j = 0; j < size; j++)
//     {
//       if (item == list2[j] && )

//     }
//   }
// }

template<typename T>
bool InArray(T* arr, T element, int size)
{
  for (int i = 0; i < size; i++)
  {
    if (arr[i] == element)
      return true;
  }
  return false;
}

// If they're the same size and each element has a unique match, 
// then the two vectors are equal
template<typename T>
bool MatchingVector(std::vector<T> v1, std::vector<T> v2)
{
  int s1 = v1.size();
  int s2 = v2.size();
  if (s1 != s2)
    return false;
  
  int matched[s1];
  for (int i = 0; i < s1; i++)
  {
    T item = v1[i];
    bool unique = true;
    for (int j = 0; j < s1; j++)
    {
      if (item == v2[j])
      {
        if (!InArray(matched, j, i))
        {
          matched[i] = j;
          unique = true;
          break;
        }
        else
        {
          unique = false;
        }
      }
    }
    if (!unique)
      return false;
  }
  return true;
}

template<typename T>
bool EqualVectors(std::vector<T> v1, std::vector<T> v2)
{
  if (v1.size() != v2.size())
    return false;
  
  for (int i = 0; i < v1.size(); i++)
  {
    if (v1[i] != v2[i])
      return false;
  }
  return true;
}

#endif // UTILS_H