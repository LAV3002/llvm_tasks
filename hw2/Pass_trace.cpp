#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
using namespace llvm;

namespace {
struct MyPass : public FunctionPass {
  static char ID;
  MyPass() : FunctionPass(ID) {}

  bool isFuncLogger(StringRef name) {
    return name == "binOptLogger" || name == "callLogger" ||
           name == "funcStartLogger" || name == "funcEndLogger" || name == "instLogger";
  }

  virtual bool runOnFunction(Function &F) {
    if (isFuncLogger(F.getName())) {
      return false;
    }
    // Dump Function
    outs() << "In a function called " << F.getName() << "\n\n";

    for (auto &B : F) {
      for (auto &I : B) {
        // Dump Instructions
        outs() << "Instruction: " << (uint64_t)(&I) << "\n";
        I.print(outs(), true);
        outs() << "\n";
      }
      outs() << "\n";
    }

    // Prepare builder for IR modification
    LLVMContext &Ctx = F.getContext();
    IRBuilder<> builder(Ctx);
    Type *retType = Type::getVoidTy(Ctx);

    // Prepare binOptLogger function
    ArrayRef<Type *> binOptParamTypes = {Type::getInt32Ty(Ctx),
                                            Type::getInt32Ty(Ctx),
                                            Type::getInt32Ty(Ctx),
                                            builder.getInt8Ty()->getPointerTo(),
                                            builder.getInt8Ty()->getPointerTo(),
                                            Type::getInt64Ty(Ctx)};
    FunctionType *binOptLogFuncType =
        FunctionType::get(retType, binOptParamTypes, false);
    FunctionCallee binOptLogFunc =
        F.getParent()->getOrInsertFunction("binOptLogger", binOptLogFuncType);

    // Prepare instLogger function
    ArrayRef<Type *> instParamTypes = {builder.getInt8Ty()->getPointerTo()};
    FunctionType *instLogFuncType =
        FunctionType::get(retType, instParamTypes, false);
    FunctionCallee instLogFunc =
        F.getParent()->getOrInsertFunction("instLogger", instLogFuncType);
    
    // Insert loggers for call, binOpt and ret instructions
    for (auto &B : F) {
      for (auto &I : B) {
        Value *valueAddr =
            ConstantInt::get(builder.getInt64Ty(), (int64_t)(&I));
        if (auto *call = dyn_cast<CallInst>(&I)) {
          // Insert before call
          builder.SetInsertPoint(call);

          // Insert a call to callLogger function
          Function *callee = call->getCalledFunction();
          if (callee && !isFuncLogger(callee->getName())) {

            // Insert a call to instLogFunc function
            Value *instName = builder.CreateGlobalStringPtr(call->getOpcodeName());
            Value *args[] = {instName};
            builder.CreateCall(instLogFunc, args);
          }
        } else {
          if (auto *phi = dyn_cast<PHINode>(&I)) {
            continue;
          }
          auto *inst = dyn_cast<Instruction>(&I);
          // Insert before call
          builder.SetInsertPoint(inst);

          // Insert a call to instLogFunc function
          Value *instName = builder.CreateGlobalStringPtr(inst->getOpcodeName());
          Value *args[] = {instName};
          builder.CreateCall(instLogFunc, args);
        }
      }
    }
    return true;
  }
};
} // namespace

char MyPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerMyPass(const PassManagerBuilder &,
                                 legacy::PassManagerBase &PM) {
  PM.add(new MyPass());
}
static RegisterStandardPasses
    RegisterMyPass(PassManagerBuilder::EP_VectorizerStart,
                   registerMyPass);