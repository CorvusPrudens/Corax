#pragma once

#ifndef COMPILER_H
#define COMPILER_H

#include <iostream>
#include <vector>
#include <memory>
#include "antlr4-runtime.h"
#include "CoraxLexer.h"
#include "CoraxParser.h"
#include "CoraxBaseVisitor.h"
#include "CoraxBaseListener.h"

#include "identifier.h"
#include "stable.h"

#include "utils.h"
#include "error.h"

using namespace antlr4;
using antlrcpp::Any;
using std::unique_ptr;
using antlr4::tree::ParseTree;

// forward declaration
class OperatorBase;

class Compiler : CoraxBaseVisitor {

  public:
    Compiler(ProcessedCode* code_, Error* err_, bool g = false);
    ~Compiler() {}

    // void Process(ProcessedCode* code_, Error* err_);
    void Complete();
    void EnableGraph(bool enable) { graphing = enable; }

    void addNodeError(ParseTree* node, string errmess);
    void addNodeWarning(ParseTree* node, string warnmess);

  // private:

    tree::ParseTreeProperty<string> storage;
    tree::ParseTreeProperty<Result> results;

    ANTLRInputStream stream;
    CoraxLexer lexer;
    CommonTokenStream tokens;
    CoraxParser parser;

    ProcessedCode* code;
    Error* err;
    unique_ptr<SymbolTable> globalTable;
    Identifier globalFunction;
    SymbolTable* currentScope;
    Identifier* currentFunction;
    // TODO -- This should probably be refactored later to not suck
    std::vector<Identifier*> currentId;
    std::vector<Type*> currentType;
    std::list<Result> tempResults;

    std::vector<int> currentIf;
    size_t totalIfs;

    std::vector<int> currentSwitch;
    size_t totalSwitches;

    std::vector<int> currentWhile;
    size_t totalWhiles;

    std::vector<int> currentFor;
    size_t totalFors;

    void PushIf() { currentIf.push_back(totalIfs++); }
    void PushSwitch() { currentSwitch.push_back(totalSwitches++); }
    void PushWhile() { currentWhile.push_back(totalWhiles++); }
    void PushFor() { currentFor.push_back(totalFors++); }

    void PopIf() { currentIf.pop_back(); }
    void PopSwitch() { currentSwitch.pop_back(); }
    void PopWhile() { currentWhile.pop_back(); }
    void PopFor() { currentFor.pop_back(); }

    string LabelIf(bool before = true);
    string LabelSwitch(bool before = true);
    string LabelWhile(bool before = true);
    string LabelFor(bool before = true);

    Result& generateResult(Identifier& id);
    Result& generateResult(Result& res);

    Graph graph;
    bool graphing = false;
    bool inherit = false;
    bool func_decl_err;
    int unnamed_inc = 0;

    void pushScope(ParseTree* rule, SymbolTable::Scope scope = SymbolTable::Scope::LOCAL, bool add_instruction = true);
    void popScope(ParseTree* rule, bool add_instruction = true);

    void operation(ParseTree* ctx, Result op1, Result op2, OperatorBase& oper);
    // unary
    void operation(ParseTree* ctx, Result op1, OperatorBase& oper);

    Any visitParse(CoraxParser::ParseContext* ctx) override;
    // Any visitTopDecl(CoraxParser::TopDeclContext* ctx) override;
    // Any visitTopFunc(CoraxParser::TopFuncContext* ctx) override;
    Any visitBlockDecl(CoraxParser::BlockDeclContext* ctx) override;
    Any visitBlockStat(CoraxParser::BlockStatContext* ctx) override;
    Any visitStat_compound(CoraxParser::Stat_compoundContext* ctx) override;
    Any visitParamList(CoraxParser::ParamListContext* ctx) override;
    Any visitTypeStd(CoraxParser::TypeStdContext* ctx) override;
    Any visitTypeStructUnion(CoraxParser::TypeStructUnionContext* ctx) override;
    Any visitTypeEnum(CoraxParser::TypeEnumContext* ctx) override;
    Any visitTypeTypedef(CoraxParser::TypeTypedefContext* ctx) override;
    Any visitFuncSpecifier(CoraxParser::FuncSpecifierContext* ctx) override;
    Any visitStorageSpecifier(CoraxParser::StorageSpecifierContext* ctx) override;
    Any visitDirId(CoraxParser::DirIdContext* ctx) override;
    Any visitPointer_item(CoraxParser::Pointer_itemContext* ctx) override;

    // Declarations
    Any visitDeclaration(CoraxParser::DeclarationContext* ctx) override;
    Any visitDeclarator(CoraxParser::DeclaratorContext* ctx) override;
    Any visitDirFunc(CoraxParser::DirFuncContext* ctx) override;
    Any visitFunc_def(CoraxParser::Func_defContext* ctx) override;
    Any visitParamDecl(CoraxParser::ParamDeclContext* ctx) override;
    Any visitParamAbst(CoraxParser::ParamAbstContext* ctx) override;

    Any visitQualConst(CoraxParser::QualConstContext* ctx) override;
    Any visitQualRestrict(CoraxParser::QualRestrictContext* ctx) override;
    Any visitQualVolatile(CoraxParser::QualVolatileContext* ctx) override;

    // Expressions
    Any visitDereference(CoraxParser::DereferenceContext *ctx) override;
    Any visitIdentifier(CoraxParser::IdentifierContext *ctx) override;
    Any visitNegation(CoraxParser::NegationContext *ctx) override;
    Any visitIncrementUnary(CoraxParser::IncrementUnaryContext *ctx) override;
    Any visitAddress(CoraxParser::AddressContext *ctx) override;
    Any visitConstInt(CoraxParser::ConstIntContext *ctx) override;
    Any visitConstFlt(CoraxParser::ConstFltContext *ctx) override;
    Any visitString(CoraxParser::StringContext *ctx) override;
    Any visitIndexing(CoraxParser::IndexingContext *ctx) override;
    Any visitIncrementPost(CoraxParser::IncrementPostContext *ctx) override;
    Any visitSizeof(CoraxParser::SizeofContext *ctx) override;
    Any visitPositive(CoraxParser::PositiveContext *ctx) override;
    Any visitDecrementPost(CoraxParser::DecrementPostContext *ctx) override;
    Any visitDecrementUnary(CoraxParser::DecrementUnaryContext *ctx) override;
    Any visitCall(CoraxParser::CallContext *ctx) override;
    Any visitSizeofType(CoraxParser::SizeofTypeContext *ctx) override;
    Any visitIndirectMember(CoraxParser::IndirectMemberContext *ctx) override;
    Any visitNegative(CoraxParser::NegativeContext *ctx) override;
    Any visitNot(CoraxParser::NotContext *ctx) override;
    Any visitMember(CoraxParser::MemberContext *ctx) override;
    Any visitGroup(CoraxParser::GroupContext *ctx) override;
    Any visitExprCast(CoraxParser::ExprCastContext *ctx) override;
    Any visitCast(CoraxParser::CastContext *ctx) override;
    Any visitMinus(CoraxParser::MinusContext *ctx) override;
    Any visitMult(CoraxParser::MultContext *ctx) override;
    Any visitMod(CoraxParser::ModContext *ctx) override;
    Any visitOr(CoraxParser::OrContext *ctx) override;
    Any visitExprMulti(CoraxParser::ExprMultiContext *ctx) override;
    Any visitNotEqual(CoraxParser::NotEqualContext *ctx) override;
    Any visitLess(CoraxParser::LessContext *ctx) override;
    Any visitBit_or(CoraxParser::Bit_orContext *ctx) override;
    Any visitPlus(CoraxParser::PlusContext *ctx) override;
    Any visitGreater_equal(CoraxParser::Greater_equalContext *ctx) override;
    Any visitDiv(CoraxParser::DivContext *ctx) override;
    Any visitEqual(CoraxParser::EqualContext *ctx) override;
    Any visitShiftLeft(CoraxParser::ShiftLeftContext *ctx) override;
    Any visitShiftRight(CoraxParser::ShiftRightContext *ctx) override;
    Any visitBit_xor(CoraxParser::Bit_xorContext *ctx) override;
    Any visitAnd(CoraxParser::AndContext *ctx) override;
    Any visitBit_and(CoraxParser::Bit_andContext *ctx) override;
    Any visitGreater(CoraxParser::GreaterContext *ctx) override;
    Any visitLess_equal(CoraxParser::Less_equalContext *ctx) override;
    Any visitExprTern(CoraxParser::ExprTernContext *ctx) override;
    Any visitTernary(CoraxParser::TernaryContext *ctx) override;
    Any visitExprAssi(CoraxParser::ExprAssiContext *ctx) override;
    Any visitAssignment(CoraxParser::AssignmentContext *ctx) override;
    Any visitAssignmentMult(CoraxParser::AssignmentMultContext *ctx) override;
    Any visitAssignmentDiv(CoraxParser::AssignmentDivContext *ctx) override;
    Any visitAssignmentMod(CoraxParser::AssignmentModContext *ctx) override;
    Any visitAssignmentPlus(CoraxParser::AssignmentPlusContext *ctx) override;
    Any visitAssignmentMinus(CoraxParser::AssignmentMinusContext *ctx) override;
    Any visitAssignmentShiftLeft(CoraxParser::AssignmentShiftLeftContext *ctx) override;
    Any visitAssignmentShiftRight(CoraxParser::AssignmentShiftRightContext *ctx) override;
    Any visitAssignmentBitAnd(CoraxParser::AssignmentBitAndContext *ctx) override;
    Any visitAssignmentBitXor(CoraxParser::AssignmentBitXorContext *ctx) override;
    Any visitAssignmentBitOr(CoraxParser::AssignmentBitOrContext *ctx) override;
    Any visitArg_expr_list(CoraxParser::Arg_expr_listContext* ctx) override;
    Any visitComma(CoraxParser::CommaContext *ctx) override;
    Any visitExprExpression(CoraxParser::ExprExpressionContext *ctx) override;
    Any visitExpr_const(CoraxParser::Expr_constContext *ctx) override;

    Any visitStatReturn(CoraxParser::StatReturnContext* ctx) override;
    Any visitStatLabeled(CoraxParser::StatLabeledContext* ctx) override;
    Any visitStatIf(CoraxParser::StatIfContext* ctx) override;
    Any visitStatIfElse(CoraxParser::StatIfElseContext* ctx) override;

    Any visitInitAssign(CoraxParser::InitAssignContext* ctx) override;
    // Any visitInitList(CoraxParser::InitListContext* ctx) override;
    Any visitInitDeclAssigned(CoraxParser::InitDeclAssignedContext* ctx) override;

};

// class CompilerListener : CoraxBaseListener {

//   public:
//     CompilerListener() {}
//     ~CompilerListener() {}

//     void Process(ANTLRInputStream* stream);

//     void enterParse(CoraxParser::ParseContext* ctx) override;
//     void enterVariable_init(CoraxParser::Variable_initContext* ctx) override;

//   private:

//     std::vector<Variable> globals;
//     CoraxParser* parse;

// };

#endif