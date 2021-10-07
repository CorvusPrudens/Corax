#pragma once

#ifndef TYPE_H
#define TYPE_H

#include "utils.h"

#include <unordered_map>
#include <vector>
#include <array>
#include <algorithm>
#include <string>

using std::string;
using std::vector;
// class Type {


// };

// We can have multiple qualifiers, so they can be
// or'd together
enum Qualifier {
  NONE = 0,
  CONST = 1,
  RESTRICT = 2,
  VOLATILE = 4,
};

class Pointer {
  public:
    Pointer(int q = Qualifier::NONE) {
      qualifiers = q;
    }
    ~Pointer() {}

    string to_string();

    void setQualifier(string q) 
    {
      int temp = qualifiers;
      if (q == "const") {
        temp |= Qualifier::CONST;
      } else if (q == "restrict") {
        temp |= Qualifier::RESTRICT;
      } else if (q == "volatile") {
        temp |= Qualifier::VOLATILE;
      }
      qualifiers = temp;
    }

    void setQualifiers(vector<string> quals)
    {
      int temp = qualifiers;
      for (auto q : quals) {
        if (q == "const") {
          if (temp & (int) Qualifier::CONST) throw 1;
          temp |= Qualifier::CONST;
        } else if (q == "restrict") {
          if (temp & (int) Qualifier::RESTRICT) throw 1;
          temp |= Qualifier::RESTRICT;
        } else if (q == "volatile") {
          if (temp & (int) Qualifier::VOLATILE) throw 1;
          temp |= Qualifier::VOLATILE;
        }
      }
      qualifiers = temp;
    }

    bool operator==(Pointer other) {
      return qualifiers == other.qualifiers;
    }

    bool operator!=(Pointer other) {
      return qualifiers != other.qualifiers;
    }

    int qualifiers;
    // Pointers will always be one word for my cpu
    static constexpr int words = 1;
};

struct Type {

  enum StorageClass {
    AUTO = 0,
    STATIC,
    TYPEDEF,
    EXTERN,
    REGISTER,
  };

  enum FunctionSpecifier {
    NONE = 0,
    INLINE = 1,
  };

  Type() {
    storageSet = false;
    name = "";
    storage = StorageClass::AUTO;
    function = FunctionSpecifier::NONE;
    qualifiers = 0;
  }

  Type(const Type& other) {
    storageSet = other.storageSet;
    storage = other.storage;
    qualifiers = other.qualifiers;
    type_specifiers = other.type_specifiers;
    function = other.function;
    pointers = other.pointers;
  }

  // Type(string n, StorageClass s = StorageClass::AUTO, int q = Qualifier::NONE) {
  //   name = n;
  //   storage = s;
  //   qualifiers = q;
  // }

  string to_string();
  size_t size();

  // bool operator==(Type& other) {
  //   return equal(other);
  // }

  // bool operator!=(Type& other) {
  //   return !equal(other);
  // }

  Type copy()
  {
    // Type newtype(name, storage, qualifiers);
    Type newtype;
    newtype.storageSet = storageSet;
    newtype.storage = storage;
    newtype.qualifiers = qualifiers;
    newtype.type_specifiers = type_specifiers;
    newtype.function = function;
    newtype.pointers = pointers;
    return newtype;
  }

  bool isConst()
  {
    return qualifiers & (Qualifier::CONST);
  }

  bool isPointer()
  {
    return pointers.size() > 0;
  }

  Type pointsTo()
  {
    if (!isPointer())
      throw(1);
    
    Type pointed = copy();
    pointed.pointers.erase(pointed.pointers.begin());
    return pointed;
  }

  void setStorage(string s)
  {
    if (s == "typedef") {
      storage = StorageClass::TYPEDEF;
    } else if (s == "auto") {
      storage = StorageClass::AUTO;
    } else if (s == "static") {
      storage = StorageClass::STATIC;
    } else if (s == "extern") {
      storage = StorageClass::EXTERN;
    } else if (s == "register") {
      storage = StorageClass::REGISTER;
    }
    storageSet = true;
  }

  void sortSpecifiers() {
    if (type_specifiers.size() < 2)
      return;
    
    std::sort(type_specifiers.begin(), type_specifiers.end());
  }

  string name;
  vector<string> type_specifiers;
  int qualifiers;
  StorageClass storage;
  FunctionSpecifier function;
  bool storageSet;
  // each star can have its own type qualifiers
  vector<Pointer> pointers;

  // private:
  bool equal(Type& other)
  {
    bool matching = true;
    if (!EqualVectors(type_specifiers, other.type_specifiers))
      return false;
    if (qualifiers != other.qualifiers || storage != other.storage || function != other.function)
      return false;
    if (!EqualVectors(pointers, other.pointers))
      return false;
    
    return true;
  }

};

struct TypeDescriptor {

  TypeDescriptor(vector<vector<string>> s, string p, unsigned int b)
  {
    specifiers = s;
    preferred = p;
    bytes = b;
  }

  TypeDescriptor(const TypeDescriptor& other) 
  {
    specifiers = other.specifiers;
    bytes = other.bytes;
    preferred = other.preferred;
  }

  string to_string() { return preferred; }

  vector<vector<string>> specifiers;
  string preferred;
  unsigned int bytes;

  // bool operator==(TypeDescriptor& other) {
  //   return equal(other);
  // }

  // bool operator!=(TypeDescriptor& other) {
  //   return !equal(other);
  // }

  bool equal(TypeDescriptor& other) {
    if (bytes != other.bytes || specifiers.size() != other.specifiers.size())
      return false;
    else
    {
      for (int i = 0; i < specifiers.size(); i++)
      {
        if (!EqualVectors(specifiers[i], other.specifiers[i]))
          return false;
      }
    }
    return true;
  }

};

bool operator==(Type& type, Type& other);
bool operator!=(Type& type, Type& other);

bool operator==(TypeDescriptor& type, TypeDescriptor& other);
bool operator!=(TypeDescriptor& type, TypeDescriptor& other);

bool operator==(Type& type, TypeDescriptor& desc);
bool operator==(TypeDescriptor& desc, Type& type);

int fetchPriority(Type t);

extern TypeDescriptor long_double_;
extern TypeDescriptor double_;
extern TypeDescriptor float_;
extern TypeDescriptor unsigned_long_long_;
extern TypeDescriptor long_long_;
extern TypeDescriptor unsigned_long_;
extern TypeDescriptor long_;
extern TypeDescriptor unsigned_;
extern TypeDescriptor int_;
extern TypeDescriptor unsigned_short_;
extern TypeDescriptor short_;
extern TypeDescriptor unsigned_char_;
extern TypeDescriptor signed_char_;
extern TypeDescriptor char_;
extern TypeDescriptor void_; 
extern vector<TypeDescriptor*> StandardTypes;

#endif

// TODO -- now we need struct, array, and union classes