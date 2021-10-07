#include <string>
#include "type.h"

using std::string;

bool operator==(Type& type, Type& other)
{
  return type.equal(other);
}

bool operator!=(Type& type, Type& other)
{
  return !type.equal(other);
}

bool operator==(TypeDescriptor& type, TypeDescriptor& other)
{
  return type.equal(other);
}

bool operator!=(TypeDescriptor& type, TypeDescriptor& other)
{
  return !type.equal(other);
}

bool operator==(Type& type, TypeDescriptor& desc) {
  for (int i = 0; i < desc.specifiers.size(); i++)
    if (EqualVectors(type.type_specifiers, desc.specifiers[i]))
      return true;
  
  return false;
}

bool operator==(TypeDescriptor& desc, Type& type) {
  for (int i = 0; i < desc.specifiers.size(); i++)
    if (EqualVectors(type.type_specifiers, desc.specifiers[i]))
      return true;
  
  return false;
}

string Pointer::to_string()
{
  string s = "";
  if (qualifiers & (int) Qualifier::CONST)
    s += "const ";
  if (qualifiers & (int) Qualifier::VOLATILE)
    s += "volatile ";
  if (qualifiers & (int) Qualifier::RESTRICT)
    s += "restrict ";
  if (s.back() == ' ')
    s = s.substr(0, s.size() - 1);
  return s + "*";
}

string Type::to_string()
{
  string s = "";
  switch (storage)
  {
    default:
    case AUTO:
      break;
    case STATIC:
      s += "static ";
      break;
    case TYPEDEF:
      s += "typedef ";
      break;
    case EXTERN:
      s += "extern ";
      break;
    case REGISTER:
      s += "register ";
      break;
  }
  if (function == INLINE)
    s += "inline ";
  if (qualifiers & (int) Qualifier::CONST)
    s += "const ";
  if (qualifiers & (int) Qualifier::VOLATILE)
    s += "volatile ";
  if (qualifiers & (int) Qualifier::RESTRICT)
    s += "restrict ";

  for (auto ts : type_specifiers)
    s += ts + " ";
  for (auto p : pointers)
    s += p.to_string() + " ";

  if (storage == TYPEDEF)
    return s + name;
  
  if (s.back() == ' ')
    s = s.substr(0, s.size() - 1);
  return s;
}

// each individual list is sorted alphabetically
TypeDescriptor long_double_({{"double", "long"}}, "long double", 16);
TypeDescriptor double_({{"double"}}, "double", 8);
TypeDescriptor float_({{"float"}}, "float", 4);
TypeDescriptor unsigned_long_long_({{"long", "long", "unsigned"}, {"int", "long", "long", "unsigned"}}, "unsigned long long", 16);
TypeDescriptor long_long_({{"long", "long"}, {"long", "long", "signed"}, {"int", "long", "long"}, {"int", "long", "long", "signed"}}, "long long", 16);
TypeDescriptor unsigned_long_({{"long", "unsigned"}, {"int", "long", "unsigned"}}, "unsigned long", 8);
TypeDescriptor long_({{"long"}, {"long", "signed"}, {"int", "long"}, {"int", "long", "signed"}}, "long", 8);
TypeDescriptor unsigned_({{"unsigned"}, {"int", "unsigned"}}, "unsigned int", 4);
TypeDescriptor int_({{"int"}, {"signed"}, {"int", "signed"}}, "int", 4);
TypeDescriptor unsigned_short_({{"short", "unsigned"}, {"int", "short", "unsigned"}}, "unsigned short", 2);
TypeDescriptor short_({{"short"}, {"short", "signed"}, {"int", "short"}, {"int", "short", "signed"}}, "short", 2);
TypeDescriptor unsigned_char_({{"char", "unsigned"}}, "unsigned char", 1);
TypeDescriptor signed_char_({{"char", "signed"}}, "signed char", 1);
TypeDescriptor char_({{"char"}}, "char", 1);
TypeDescriptor void_({{"void"}}, "void", 1); // it'll be easier to consider this 1

vector<TypeDescriptor*> StandardTypes = {
  &long_double_,
  &double_,
  &float_,
  &unsigned_long_long_,
  &long_long_,
  &unsigned_long_,
  &long_,
  &unsigned_,
  &int_,
  &unsigned_short_,
  &short_,
  &unsigned_char_,
  &signed_char_,
  &char_,
  &void_,
};

int fetchPriority(Type t)
{
  for (int i = 0; i < StandardTypes.size(); i++)
  {
    if (t == *StandardTypes[i])
      return i;
  }
  throw 1;
}

// TODO -- make this work with typedefs and structs / unions
size_t Type::size()
{
  for (int i = 0; i < StandardTypes.size(); i++)
  {
    if (*this == *StandardTypes[i])
      return StandardTypes[i]->bytes;
  }
  throw 1;
}