
#ifndef CORVASSEMBLY_H
#define CORVASSEMBLY_H

#include "target.h"
#include "compiler.h"


class CorvassemblyTarget : public BaseTarget {

  public:
    CorvassemblyTarget(Compiler* c);
    ~CorvassemblyTarget() {}

    void StandardInstruction(Instruction& inst, string mnemonic);

    void TranslateAdd(Instruction& inst) override;
    void TranslateSub(Instruction& inst) override;
    void TranslateMult(Instruction& inst) override;
    void TranslateDiv(Instruction& inst) override;
    void TranslateMod(Instruction& inst) override;

    void TranslateShiftl(Instruction& inst) override;
    void TranslateShiftr(Instruction& inst) override;
    void TranslateBitAnd(Instruction& inst) override;
    void TranslateBitXor(Instruction& inst) override;
    void TranslateBitOr(Instruction& inst) override;

    void TranslateAnd(Instruction& inst) override;
    void TranslateOr(Instruction& inst) override;
    void TranslateCmp(Instruction& inst) override;

    void TranslateDeref(Instruction& inst) override;
    void TranslateNot(Instruction& inst) override;
    void TranslateNegate(Instruction& inst) override;
    void TranslateNegative(Instruction& inst) override;
    void TranslateConvert(Instruction& inst) override;
    void TranslateAssign(Instruction& inst) override;

    void TranslateCall(Instruction& inst) override;
    void TranslateSetup(Instruction& inst) override;
    void TranslateReturnPrep(Instruction& inst) override;
    void TranslateReturn(Instruction& inst) override;

    void TranslateIf(Instruction& inst) override;
    void TranslateLabel(Instruction& inst) override;
    void TranslateConditional(Instruction& inst) override;

    void TranslateScopeBegin(Instruction& inst) override;
    void TranslateScopeEnd(Instruction& inst) override;

    void TranslateDeclare(Instruction& inst) override;

    void TranslateStore(Register& reg, Instruction& inst) override;
    void TranslateStore(Register& reg, Instruction& inst, Identifier& id) override;
    void TranslateLoad(Register& reg, Instruction& inst, Result& res) override;

    void SaveUsedRegisters(Identifier& function) override;
    void RestoreUsedRegisters(Identifier& function) override;
};

#endif