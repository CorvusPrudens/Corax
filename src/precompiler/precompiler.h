#pragma once

#include <vector>
#include <filesystem>
#include <regex>

#include "antlr4-runtime.h"
#include "PreLexer.h"
#include "PreParser.h"
#include "PreVisitor.h"
#include "PreBaseVisitor.h"

#include "PreExprLexer.h"
#include "PreExprParser.h"
#include "PreExprVisitor.h"
#include "PreExprBaseVisitor.h"

#include "error.h"
#include "utils.h"

using namespace antlr4;
using std::string;
using std::filesystem::path;
using std::regex;
using antlrcpp::Any;

// class PreprocessListener : PreBaseListener {

//   public:
//     PreprocessListener() {}
//     ~PreprocessListener() {}

//     struct Import {
//       string name;
//       std::filesystem::path parent;
//       std::vector<string> imports;
//     };

//     struct Macro {
//       string name;
//       string content;
//     };

//     string Process(string filename, Error* err, bool top = true, string parent = ".");

//   private:
  
//     std::vector<Macro> macros;
//     void enterDirective(PreParser::DirectiveContext *ctx) override;
//     Macro getMacro(string macro_name);
//     bool isDefined(string macro_name) { return getMacro(macro_name).name != ""; }
//     void handleElse(PreParser::If_Context *ifdef, bool prev_condition);
//     void handleElse(PreParser::Ifdef_Context *ifdef, bool prev_condition);
//     void handleElse(PreParser::Ifndef_Context *ifdef, bool prev_condition);
// };

struct Expression {
  bool   isValue;
  double fvalue;
};

// class Evaluator : PreExprBaseVisitor {
//   public:
//     Evaluator(ProcessedCode* code_) {
//       code = code_;
//     }
//     ~Evaluator() {}

//     Expression eval(string expression);
//     string     expandMacros(string name);

//   private:
//     tree::ParseTreeProperty<Expression> expr_vals;
//     tree::ParseTreeProperty<string> macro_strings;
//     ProcessedCode* code;
    
//     Any visitDirName(PreParser::DirNameContext* ctx) override;
//     Any visitDirAny(PreParser::DirAnyContext* ctx) override;
//     Any visitDirHash(PreParser::DirHashContext* ctx) override;
//     Any visitDirDHash(PreParser::DirDHashContext* ctx) override;
// };

class Evaluator : PreExprBaseVisitor {
  public:
    Evaluator() {}
    ~Evaluator() {}

    Expression eval(string text, ProcessedCode* code_);

  private:
    tree::ParseTreeProperty<Expression> vals;
    ProcessedCode* code;

    Any visitStatement(PreExprParser::StatementContext* ctx) override;

    Any visitDefined(PreExprParser::DefinedContext* ctx) override;
    Any visitNegation(PreExprParser::NegationContext* ctx) override;
    Any visitPositive(PreExprParser::PositiveContext* ctx) override;
    Any visitNegative(PreExprParser::NegativeContext* ctx) override;
    Any visitNot(PreExprParser::NotContext* ctx) override;
    Any visitMult(PreExprParser::MultContext* ctx) override;
    Any visitDiv(PreExprParser::DivContext* ctx) override;
    Any visitMod(PreExprParser::ModContext* ctx) override;
    Any visitPlus(PreExprParser::PlusContext* ctx) override;

    Any visitMinus(PreExprParser::MinusContext* ctx) override;
    Any visitShift_left(PreExprParser::Shift_leftContext* ctx) override;
    Any visitShift_right(PreExprParser::Shift_rightContext* ctx) override;
    Any visitLess(PreExprParser::LessContext* ctx) override;
    Any visitGreater(PreExprParser::GreaterContext* ctx) override;
    Any visitLess_equal(PreExprParser::Less_equalContext* ctx) override;
    Any visitGreater_equal(PreExprParser::Greater_equalContext* ctx) override;
    Any visitEqual(PreExprParser::EqualContext* ctx) override;
    Any visitNot_equal(PreExprParser::Not_equalContext* ctx) override;

    Any visitBit_and(PreExprParser::Bit_andContext* ctx) override;
    Any visitBit_xor(PreExprParser::Bit_xorContext* ctx) override;
    Any visitBit_or(PreExprParser::Bit_orContext* ctx) override;
    Any visitAnd(PreExprParser::AndContext* ctx) override;
    Any visitOr(PreExprParser::OrContext* ctx) override;
    Any visitConst(PreExprParser::ConstContext* ctx) override;
    Any visitGroup(PreExprParser::GroupContext* ctx) override;

    Any visitDec(PreExprParser::DecContext* ctx) override;
    Any visitBin(PreExprParser::BinContext* ctx) override;
    Any visitHex(PreExprParser::HexContext* ctx) override;
    Any visitOct(PreExprParser::OctContext* ctx) override;
    Any visitFlt(PreExprParser::FltContext* ctx) override;

};

// we'll just use tree annotation because any seems broken
class Expander : PreBaseVisitor {
  public:
    Expander() {}
    ~Expander() {}

    string expand(string text, ProcessedCode* code_);

  private:
    tree::ParseTreeProperty<Expression> expr_vals;
    tree::ParseTreeProperty<string> macro_strings;
    ProcessedCode* code;
    CommonTokenStream* tokens;
    TokenStreamRewriter* rewriter;
    
    Any visitAnyName(PreParser::AnyNameContext* ctx) override;
    Any visitAnyPass(PreParser::AnyPassContext* ctx) override;

    Any defaultResult() override {
      string temp = "";
      Any any(temp);
      return any;
    }
    Any aggregateResult(Any left, const Any &nextResult) override {
      // string s1 = left.as<string>();
      // string s2 = nextResult.as<string>();
      // string concat(s1 + s2);
      // Any any(concat);
      // return any;
      return nextResult;
    }

    Any visitChildren(tree::ParseTree *node) override {
      string accum = "";
      size_t n = node->children.size();
      for (size_t i = 0; i < n; i++) {
        // if (!shouldVisitNextChild(node, result)) {
        //   break;
        // }

        node->children[i]->accept(this);
      }

      // for (size_t i = 0; i < n; i++) {
      //   accum += macro_strings.get(node->children[i]);
      // }

      // macro_strings.put(node, accum);

      return nullptr;
    }
};

class Precompiler : PreBaseVisitor {

  public:

    Precompiler(ProcessedCode* processed, path file_, Error* err_)
    { 
      output = processed;
      file = file_;
      err = err_;
    };

    ~Precompiler() {}

    void Process(int depth_ = 0);

  private:

    CommonTokenStream* tokens;
    TokenStreamRewriter* rewriter;
    Evaluator eval;

    tree::ParseTreeProperty<Expression> vals;

    static constexpr int max_expansion = 32;
    static constexpr int max_include_depth = 32;
    string expandMacro(string name, int depth=0);
    
    Expression evaluateExpr(string expr);

    void addText(string text, int line_number);

    ProcessedCode* output;
    path file;

    static regex id;
    Error* err;

    int depth = 0;

    string expandExpr(PreParser::Anything_exprContext* ctx);

    Any visitTopDirective(PreParser::TopDirectiveContext* ctx) override;
    Any visitIf_(PreParser::If_Context* ctx) override;
    Any visitObject(PreParser::ObjectContext* ctx) override;
    Any visitFunction(PreParser::FunctionContext* ctx) override;
    Any visitAnyName(PreParser::AnyNameContext* ctx) override;
    Any visitAnyPass(PreParser::AnyPassContext* ctx) override;
    Any visitAnyNum(PreParser::AnyNumContext* ctx) override;
    Any visitAnyNewline(PreParser::AnyNewlineContext* ctx) override;
    Any visitAnyEof(PreParser::AnyEofContext* ctx) override;

    Any visitDirective(PreParser::DirectiveContext* ctx) override;

    Any visitString(PreParser::StringContext* ctx) override;
    Any visitLibrary(PreParser::LibraryContext* ctx) override;

    Any visitBin(PreParser::BinContext* ctx) override;
    Any visitHex(PreParser::HexContext* ctx) override;
    Any visitSci(PreParser::SciContext* ctx) override;
    Any visitOct(PreParser::OctContext* ctx) override;

    Any visitElif_(PreParser::Elif_Context* ctx) override;
    Any visitIfdef_(PreParser::Ifdef_Context* ctx) override;
    Any visitIfndef_(PreParser::Ifndef_Context* ctx) override;
    Any visitUndef_(PreParser::Undef_Context* ctx) override;

    Any visitLine_(PreParser::Line_Context* ctx) override;
    Any visitError_(PreParser::Error_Context* ctx) override;
    Any visitPragma_(PreParser::Pragma_Context* ctx) override;

};