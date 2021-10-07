
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <regex>

#include "precompiler.h"
#include "sar.h"
#include "error.h"
#include "utils.h"

using std::string;
using std::regex;
using antlrcpp::Any;

Any Evaluator::visitStatement(PreExprParser::StatementContext* ctx)
{
  visitChildren(ctx);
  vals.put(ctx, vals.get(ctx->expression()));
  return nullptr;
}

Any Evaluator::visitDefined(PreExprParser::DefinedContext* ctx) {
  string name = ctx->NAME()->getText();
  Expression exp;
  exp.isValue = true;
  try
  {
    code->getMacro(name);
    exp.fvalue = 1;
  }
  catch (int e)
  {
    exp.fvalue = 0;
  }
  vals.put(ctx, exp);
  return nullptr;
}

Any Evaluator::visitNegation(PreExprParser::NegationContext* ctx) {
  Expression exp{true, 0};
  visitChildren(ctx);
  if (!vals.get(ctx->expression()).isValue)
  {
    exp.isValue = false;
    vals.put(ctx, exp);
    return nullptr;
  }

  Expression child = vals.get(ctx->expression());
  exp.fvalue = child.fvalue != 0 ? 0 : 1;

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitPositive(PreExprParser::PositiveContext* ctx) {
  Expression exp{true, 0};
  visitChildren(ctx);
  if (!vals.get(ctx->expression()).isValue)
  {
    exp.isValue = false;
    vals.put(ctx, exp);
    return nullptr;
  }

  // this has no effect right??
  Expression child = vals.get(ctx->expression());
  exp.fvalue = child.fvalue;

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitNegative(PreExprParser::NegativeContext* ctx) {
  Expression exp{true, 0};
  visitChildren(ctx);
  if (!vals.get(ctx->expression()).isValue)
  {
    exp.isValue = false;
    vals.put(ctx, exp);
    return nullptr;
  }

  Expression child = vals.get(ctx->expression());
  exp.fvalue = -child.fvalue;

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitNot(PreExprParser::NotContext* ctx) {
  Expression exp{true, 0};
  visitChildren(ctx);
  if (!vals.get(ctx->expression()).isValue)
  {
    exp.isValue = false;
    vals.put(ctx, exp);
    return nullptr;
  }

  Expression child = vals.get(ctx->expression());
  int intermed = child.fvalue;
  exp.fvalue = ~intermed;

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitMult(PreExprParser::MultContext* ctx) {
  Expression exp{true, 0};
  visitChildren(ctx);
  Expression lhs = vals.get(ctx->children[0]);
  Expression rhs = vals.get(ctx->children[2]);
  if (!lhs.isValue || !rhs.isValue)
  {
    exp.isValue = false;
    vals.put(ctx, exp);
    return nullptr;
  }

  exp.fvalue = lhs.fvalue * rhs.fvalue;

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitDiv(PreExprParser::DivContext* ctx) {
  Expression exp{true, 0};
  visitChildren(ctx);
  Expression lhs = vals.get(ctx->children[0]);
  Expression rhs = vals.get(ctx->children[2]);
  if (!lhs.isValue || !rhs.isValue)
  {
    exp.isValue = false;
    vals.put(ctx, exp);
    return nullptr;
  }

  exp.fvalue = lhs.fvalue / rhs.fvalue;

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitMod(PreExprParser::ModContext* ctx) {
  Expression exp{true, 0};
  visitChildren(ctx);
  Expression lhs = vals.get(ctx->children[0]);
  Expression rhs = vals.get(ctx->children[2]);
  if (!lhs.isValue || !rhs.isValue)
  {
    exp.isValue = false;
    vals.put(ctx, exp);
    return nullptr;
  }

  exp.fvalue = (int) lhs.fvalue % (int) rhs.fvalue;

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitPlus(PreExprParser::PlusContext* ctx) {
  Expression exp{true, 0};
  visitChildren(ctx);
  Expression lhs = vals.get(ctx->children[0]);
  Expression rhs = vals.get(ctx->children[2]);
  if (!lhs.isValue || !rhs.isValue)
  {
    exp.isValue = false;
    vals.put(ctx, exp);
    return nullptr;
  }

  exp.fvalue = lhs.fvalue + rhs.fvalue;

  vals.put(ctx, exp);
  return nullptr;
}

Any Evaluator::visitMinus(PreExprParser::MinusContext* ctx) {
  Expression exp{true, 0};
  visitChildren(ctx);
  Expression lhs = vals.get(ctx->children[0]);
  Expression rhs = vals.get(ctx->children[2]);
  if (!lhs.isValue || !rhs.isValue)
  {
    exp.isValue = false;
    vals.put(ctx, exp);
    return nullptr;
  }

  exp.fvalue = lhs.fvalue - rhs.fvalue;

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitShift_left(PreExprParser::Shift_leftContext* ctx) {
  Expression exp{true, 0};
  visitChildren(ctx);
  Expression lhs = vals.get(ctx->children[0]);
  Expression rhs = vals.get(ctx->children[2]);
  if (!lhs.isValue || !rhs.isValue)
  {
    exp.isValue = false;
    vals.put(ctx, exp);
    return nullptr;
  }

  exp.fvalue = (int) lhs.fvalue << (int) rhs.fvalue;

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitShift_right(PreExprParser::Shift_rightContext* ctx) {
  Expression exp{true, 0};
  visitChildren(ctx);
  Expression lhs = vals.get(ctx->children[0]);
  Expression rhs = vals.get(ctx->children[2]);
  if (!lhs.isValue || !rhs.isValue)
  {
    exp.isValue = false;
    vals.put(ctx, exp);
    return nullptr;
  }

  exp.fvalue = (int) lhs.fvalue >> (int) rhs.fvalue;

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitLess(PreExprParser::LessContext* ctx) {
  Expression exp{true, 0};
  visitChildren(ctx);
  Expression lhs = vals.get(ctx->children[0]);
  Expression rhs = vals.get(ctx->children[2]);
  if (!lhs.isValue || !rhs.isValue)
  {
    exp.isValue = false;
    vals.put(ctx, exp);
    return nullptr;
  }

  exp.fvalue = lhs.fvalue < rhs.fvalue;

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitGreater(PreExprParser::GreaterContext* ctx) {
  Expression exp{true, 0};
  visitChildren(ctx);
  Expression lhs = vals.get(ctx->children[0]);
  Expression rhs = vals.get(ctx->children[2]);
  if (!lhs.isValue || !rhs.isValue)
  {
    exp.isValue = false;
    vals.put(ctx, exp);
    return nullptr;
  }

  exp.fvalue = lhs.fvalue > rhs.fvalue;

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitLess_equal(PreExprParser::Less_equalContext* ctx) {
  Expression exp{true, 0};
  visitChildren(ctx);
  Expression lhs = vals.get(ctx->children[0]);
  Expression rhs = vals.get(ctx->children[2]);
  if (!lhs.isValue || !rhs.isValue)
  {
    exp.isValue = false;
    vals.put(ctx, exp);
    return nullptr;
  }

  exp.fvalue = lhs.fvalue <= rhs.fvalue;

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitGreater_equal(PreExprParser::Greater_equalContext* ctx) {
  Expression exp{true, 0};
  visitChildren(ctx);
  Expression lhs = vals.get(ctx->children[0]);
  Expression rhs = vals.get(ctx->children[2]);
  if (!lhs.isValue || !rhs.isValue)
  {
    exp.isValue = false;
    vals.put(ctx, exp);
    return nullptr;
  }

  exp.fvalue = lhs.fvalue <= rhs.fvalue;

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitEqual(PreExprParser::EqualContext* ctx) {
  Expression exp{true, 0};
  visitChildren(ctx);
  Expression lhs = vals.get(ctx->children[0]);
  Expression rhs = vals.get(ctx->children[2]);
  if (!lhs.isValue || !rhs.isValue)
  {
    exp.isValue = false;
    vals.put(ctx, exp);
    return nullptr;
  }

  exp.fvalue = lhs.fvalue == rhs.fvalue;

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitNot_equal(PreExprParser::Not_equalContext* ctx) {
  Expression exp{true, 0};
  visitChildren(ctx);
  Expression lhs = vals.get(ctx->children[0]);
  Expression rhs = vals.get(ctx->children[2]);
  if (!lhs.isValue || !rhs.isValue)
  {
    exp.isValue = false;
    vals.put(ctx, exp);
    return nullptr;
  }

  exp.fvalue = lhs.fvalue != rhs.fvalue;

  vals.put(ctx, exp);
  return nullptr;
}

Any Evaluator::visitBit_and(PreExprParser::Bit_andContext* ctx) {
  Expression exp{true, 0};
  visitChildren(ctx);
  Expression lhs = vals.get(ctx->children[0]);
  Expression rhs = vals.get(ctx->children[2]);
  if (!lhs.isValue || !rhs.isValue)
  {
    exp.isValue = false;
    vals.put(ctx, exp);
    return nullptr;
  }

  exp.fvalue = (int) lhs.fvalue & (int) rhs.fvalue;

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitBit_xor(PreExprParser::Bit_xorContext* ctx) {
  Expression exp{true, 0};
  visitChildren(ctx);
  Expression lhs = vals.get(ctx->children[0]);
  Expression rhs = vals.get(ctx->children[2]);
  if (!lhs.isValue || !rhs.isValue)
  {
    exp.isValue = false;
    vals.put(ctx, exp);
    return nullptr;
  }

  exp.fvalue = (int) lhs.fvalue ^ (int) rhs.fvalue;

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitBit_or(PreExprParser::Bit_orContext* ctx) {
  Expression exp{true, 0};
  visitChildren(ctx);
  Expression lhs = vals.get(ctx->children[0]);
  Expression rhs = vals.get(ctx->children[2]);
  if (!lhs.isValue || !rhs.isValue)
  {
    exp.isValue = false;
    vals.put(ctx, exp);
    return nullptr;
  }

  exp.fvalue = (int) lhs.fvalue | (int) rhs.fvalue;

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitAnd(PreExprParser::AndContext* ctx) {
  Expression exp{true, 0};
  visitChildren(ctx);
  Expression lhs = vals.get(ctx->children[0]);
  Expression rhs = vals.get(ctx->children[2]);
  if (!lhs.isValue || !rhs.isValue)
  {
    exp.isValue = false;
    vals.put(ctx, exp);
    return nullptr;
  }

  exp.fvalue = lhs.fvalue && rhs.fvalue;

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitOr(PreExprParser::OrContext* ctx) {
  Expression exp{true, 0};
  visitChildren(ctx);
  Expression lhs = vals.get(ctx->children[0]);
  Expression rhs = vals.get(ctx->children[2]);
  if (!lhs.isValue || !rhs.isValue)
  {
    exp.isValue = false;
    vals.put(ctx, exp);
    return nullptr;
  }

  exp.fvalue = lhs.fvalue || rhs.fvalue;

  vals.put(ctx, exp);
  return nullptr;
}

Any Evaluator::visitConst(PreExprParser::ConstContext* ctx) {
  visitChildren(ctx);
  vals.put(ctx, vals.get(ctx->number()));
  return nullptr;
}
Any Evaluator::visitGroup(PreExprParser::GroupContext* ctx) {
  visitChildren(ctx);
  vals.put(ctx, vals.get(ctx->expression()));
  return nullptr;
}

Any Evaluator::visitDec(PreExprParser::DecContext* ctx) {
  Expression exp{true, 0};
  
  exp.fvalue = stoi(ctx->getText());

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitBin(PreExprParser::BinContext* ctx) {
  Expression exp{true, 0};
  
  exp.fvalue = stoi(ctx->getText().substr(2), nullptr, 2);

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitHex(PreExprParser::HexContext* ctx) {
  Expression exp{true, 0};
  
  exp.fvalue = stoi(ctx->getText().substr(2), nullptr, 16);

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitOct(PreExprParser::OctContext* ctx) {
  Expression exp{true, 0};
  
  exp.fvalue = stoi(ctx->getText().substr(1), nullptr, 8);

  vals.put(ctx, exp);
  return nullptr;
}
Any Evaluator::visitFlt(PreExprParser::FltContext* ctx) {
  Expression exp{true, 0};
  
  exp.fvalue = stof(ctx->getText());

  vals.put(ctx, exp);
  return nullptr;
}

Expression Evaluator::eval(string text, ProcessedCode* code_)
{
  code = code_;

  ANTLRInputStream input(text);
  PreExprLexer lexer(&input);
  CommonTokenStream tokens_(&lexer);
  PreExprParser parser(&tokens_);
  tree::ParseTree *tree = parser.statement();
  visit(tree);

  Expression result = vals.get(tree);

  // TODO -- make this clearable
  // vals._annotations.clear();

  return result;
}

regex Precompiler::id("\\b[A-Za-z_][A-Za-z_0-9]*\\b");

void Precompiler::addText(string text, int line_number)
{
  if (text == "\n") {
    output->lines.add(line_number, output->current_line, file.native());
    output->current_line++;
  }
  output->current_line++;
}

// // To avoid building a bunch of trees...
// // Recursively expands macros given current symbol table
// // This will need to be updated for function-like macros!!
// string Precompiler::expandMacro(string sequence, int depth)
// {

//   if (depth > max_expansion)
//   {
//     // TODO -- add an error message
//     // simple debug for now
//     std::cout << "you suck at this";
//     return sequence;
//   }

//   string input  = sequence;
//   string output = sequence.substr();
//   string previous_token = "";

//   auto b = std::sregex_iterator(input.begin(), input.end(), id);
//   auto e = std::sregex_iterator();
//   for (auto iter = b; b != e; ++b)
//   {
//     std::smatch match = *iter;
//     try {
//       string matched = match.str();
//       string replacement = getMacro(matched);
//       replacement = expandMacro(replacement, depth + 1);
//       output = replace(output, matched, replacement);
//     } catch (int e) {
//       // means matched identifier wasn't a macro
//     }
//   }

//   return output;
// }

// Expression Precompiler::evaluateExpr(string expr)
// {
//   string expanded = expandMacro(expr);


// }

Any Precompiler::visitTopDirective(PreParser::TopDirectiveContext* ctx)
{
  return visitChildren(ctx);
}


// TODO -- add library search path options
Any Precompiler::visitLibrary(PreParser::LibraryContext* ctx)
{

  string name = ctx->LIBRARY()->getText();
  path new_file = name.substr(1, name.size() - 2);

  // this will need to be updated!!
  new_file = file.parent_path() / new_file;

  Precompiler pre(output, new_file, err);
  pre.Process(depth + 1);

  return nullptr;
}

Any Precompiler::visitString(PreParser::StringContext* ctx)
{
  string name = ctx->STRING()->getText();
  path new_file = name.substr(1, name.size() - 2);

  // this will need to be updated!!
  new_file = file.parent_path() / new_file;

  Precompiler pre(output, new_file, err);
  pre.Process(depth + 1);

  return nullptr;
}

Any Precompiler::visitIf_(PreParser::If_Context* ctx)
{
  // return visitChildren(ctx);
  string expr = tokens->getText(ctx->anything_expr());

  // std::cout << "If: " << expr << "\n";
  // Evaluator eval(output);
  string expansion = expandExpr(ctx->anything_expr());

  // std::cout << "EXPANSION: " << expansion << "\n";

  Expression exp = eval.eval(expansion + "\n", output);

  if (!exp.isValue)
  {
    // TODO -- insert error here
    return nullptr;
  }

  if (exp.fvalue)
  {
    for (auto stuff : ctx->anything())
      visit(stuff);
  }
  else
  {
    for (auto alt : ctx->elif_())
    {
      visit(alt);
      if (vals.get(alt).fvalue)
      {
        // statement complete
        return nullptr;
      }
    }
    if (ctx->else_() != nullptr)
      visit(ctx->else_());
  }

  return nullptr;
}

Any Precompiler::visitElif_(PreParser::Elif_Context* ctx)
{
  // return visitChildren(ctx);
  string expr = tokens->getText(ctx->anything_expr());

  // std::cout << "If: " << expr << "\n";
  // Evaluator eval(output);
  string expansion = expandExpr(ctx->anything_expr());

  // std::cout << "EXPANSION: " << expansion << "\n";

  Expression exp = eval.eval(expansion + "\n", output);

  vals.put(ctx, exp);

  if (!exp.isValue)
  {
    // TODO -- insert error here
    return nullptr;
  }

  if (exp.fvalue)
  {
    for (auto stuff : ctx->anything())
      visit(stuff);
  }

  return nullptr;
}

Any Precompiler::visitIfdef_(PreParser::Ifdef_Context* ctx)
{
  try
  {
    output->getMacro(ctx->NAME()->getText());
    for (auto stuff : ctx->anything())
      visit(stuff);
  }
  catch (int e)
  {
    if (ctx->else_() != nullptr)
      visit(ctx->else_());
  }
  return nullptr;
}

Any Precompiler::visitIfndef_(PreParser::Ifndef_Context* ctx)
{
  try
  {
    output->getMacro(ctx->NAME()->getText());
    if (ctx->else_() != nullptr)
      visit(ctx->else_());
  }
  catch (int e)
  {
    for (auto stuff : ctx->anything())
      visit(stuff);
  }
  return nullptr;
}

Any Precompiler::visitUndef_(PreParser::Undef_Context* ctx)
{
  output->removeMacro(ctx->NAME()->getText());
  return nullptr;
}

// TODO -- add line support
Any Precompiler::visitLine_(PreParser::Line_Context* ctx)
{
  return nullptr;
}

Any Precompiler::visitError_(PreParser::Error_Context* ctx)
{
  // TODO -- insert error
  string errmess = ctx->STRING()->getText();
  errmess = errmess.substr(1, errmess.length() - 1);
  return nullptr;
}

// TODO -- add pragma support
Any Precompiler::visitPragma_(PreParser::Pragma_Context* ctx)
{
  // string errmess = ctx->STRING()->getText();
  // errmess = errmess.substr(1, errmess.length() - 1);
  return nullptr;
}

Any Precompiler::visitObject(PreParser::ObjectContext* ctx)
{
  string expansion = expandExpr(ctx->anything_expr());
  output->addMacro(ctx->NAME()->getText(), expansion);
  return nullptr;
}

// TODO -- this needs more work
Any Precompiler::visitFunction(PreParser::FunctionContext* ctx)
{

  // First, verify it really is function-like
  int p1 = ctx->children[2]->getSourceInterval().a;
  int p2 = ctx->children[3]->getSourceInterval().a;
  if (p1 + 1 == p2)
  {
    // actually function-like
    // TODO -- put warning here for now
  }
  else
  {
    // object-like in disguise
    string expansion = expandExpr(ctx->anything_expr());

    // int start = ctx->LP()->getSourceInterval().a;
    // int end = ctx->NEWLINE()->getSourceInterval().a;
    // string expansion = rewriter->getText(start, end);
    output->addMacro(ctx->NAME()[0]->getText(), expansion);
  }

  // string expansion = expandExpr(ctx->anything_expr());
  // output->addMacro(ctx->define_()->NAME()->getText(), expansion);
  return nullptr;
}

string Precompiler::expandExpr(PreParser::Anything_exprContext* ctx)
{
  string expansion = "";

  if (ctx != nullptr)
  {
    visit(ctx);
    expansion = rewriter->getText(ctx->getSourceInterval());
  }

  return expansion;
}

Any Precompiler::visitAnyName(PreParser::AnyNameContext* ctx)
{
  // we only need one level of expansion!
  try
  {
    string replace = output->getMacro(ctx->getText());
    rewriter->replace(ctx->getStart(), ctx->getStop(), replace);
  }
  catch (int e)
  {
    // pass
  }

  return nullptr;
}

Any Precompiler::visitAnyNum(PreParser::AnyNumContext* ctx)
{
  return visit(ctx->children[0]);
}

Any Precompiler::visitAnyPass(PreParser::AnyPassContext* ctx)
{
  return nullptr;
}

Any Precompiler::visitBin(PreParser::BinContext* ctx)
{

  int value = stoi(ctx->getText().substr(2), nullptr, 2);
  string replace = std::to_string(value);
  rewriter->replace(ctx->getStart(), ctx->getStop(), replace);

  return nullptr;
}

Any Precompiler::visitHex(PreParser::HexContext* ctx)
{

  int value = stoi(ctx->getText().substr(2), nullptr, 16);
  string replace = std::to_string(value);
  rewriter->replace(ctx->getStart(), ctx->getStop(), replace);

  return nullptr;
}

Any Precompiler::visitSci(PreParser::SciContext* ctx)
{
  int value = stof(ctx->getText());
  string replace = std::to_string(value);
  rewriter->replace(ctx->getStart(), ctx->getStop(), replace);

  return nullptr;
}

Any Precompiler::visitOct(PreParser::OctContext* ctx)
{
  int value = stoi(ctx->getText().substr(1), nullptr, 8);
  string replace = std::to_string(value);
  rewriter->replace(ctx->getStart(), ctx->getStop(), replace);

  return nullptr;
}

// TODO -- make error condition for directive that
// doesn't start on a new line
Any Precompiler::visitDirective(PreParser::DirectiveContext* ctx)
{
  // int start = ctx->getSourceInterval().a;
  // if (start != 0)
  // {
  //   string token = tokens->getTokens(start - 1, start)[0]->getText();
  //   std::cout << "FOUND NON NEWLINE: " << ctx->getText() << "CHAR: " << (int) token[0] << "\n";
  // }
  // if (start != 0 && tokens->getTokens(start - 1, start)[0]->getText() != "\n")
  // {
  //   std::cout << "ERROR!!!";
  //   return nullptr;
  // }
  return visitChildren(ctx);
}

// NOTE -- this is where the line adding happens
// TODO -- the current whitespace solution is a bit of a hack and
// can result in erroneous messages! (single line comment eats newline)
Any Precompiler::visitAnyNewline(PreParser::AnyNewlineContext* ctx)
{
  visitChildren(ctx);
  string line = rewriter->getText(ctx->getSourceInterval());
  string whitespace = "";
  auto toks = tokens->getHiddenTokensToLeft(ctx->getStart()->getTokenIndex());
  for (auto tok : toks)
  {
    whitespace += tok->getText();
  }

  // This is a bit complicated, but we're just seeing if the hidden tokens
  // themselves have newlines, winding back the line number to add the 
  // correct lines, then only adding the whitespace that's left to the
  // actual line we're adding. This is to preserve the whitespace position
  // of the actual line 
  // (TODO -- just make it rewind to a newline instead of
  // all hidden tokens)
  int real_line = ctx->start->getLine();
  for (int i = 0; i < whitespace.length(); i++)
  {
    if (whitespace[i] == '\n')
    {
      real_line--;
    }
  }
  int prevnew = 0;
  for (int i = 0; i < whitespace.length(); i++)
  {
    if (whitespace[i] == '\n')
    {
      output->addLine(real_line++, file.native(), whitespace.substr(prevnew, i + 1 - prevnew));
      prevnew = i + 1;
    }
  }

  output->addLine(ctx->start->getLine(), file.native(), whitespace.substr(prevnew) + line);
  return nullptr;
}

Any Precompiler::visitAnyEof(PreParser::AnyEofContext* ctx)
{
  visitChildren(ctx);
  string line = rewriter->getText(ctx->getSourceInterval()) + "\n";
  string whitespace = "";
  auto toks = tokens->getHiddenTokensToLeft(ctx->getStart()->getTokenIndex());
  for (auto tok : toks)
  {
    whitespace += tok->getText();
  }

  int real_line = ctx->start->getLine();
  for (int i = 0; i < whitespace.length(); i++)
  {
    if (whitespace[i] == '\n')
    {
      real_line--;
    }
  }
  int prevnew = 0;
  for (int i = 0; i < whitespace.length(); i++)
  {
    if (whitespace[i] == '\n')
    {
      output->addLine(real_line++, file.native(), whitespace.substr(prevnew, i + 1 - prevnew));
      prevnew = i + 1;
    }
  }

  output->addLine(ctx->start->getLine(), file.native(), whitespace.substr(prevnew) + line);
  return nullptr;
}

void Precompiler::Process(int depth_)
{
  depth = depth_;
  if (depth > max_include_depth)
  {
    // TODO -- make proper error
    std::cout << "Import depth exceeded " << max_include_depth << ". Possible recursive imports.\n";
    exit(1);
  }
  std::ifstream stream;
  stream.open(file.native());
  ANTLRInputStream input(stream);
  PreLexer lexer(&input);
  CommonTokenStream tokens_(&lexer);
  tokens = &tokens_;

  TokenStreamRewriter rewriter_(tokens);
  rewriter = &rewriter_;

  // CommonTokenStream tokens(&lexer);
  PreParser parser(&tokens_);

  tree::ParseTree *tree = parser.parse();
  // tree::ParseTreeWalker::DEFAULT.visit(this, tree);
  visit(tree);
}