#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"
#include "llvm/IR/Function.h"

using namespace llvm;

namespace {

#define DEBUG_TYPE "irmodel"                                                   

class IRModelPass : public FunctionPass {
       public:
	static char ID;
	IRModelPass() : FunctionPass(ID) {}

	bool runOnFunction(Function &F) override {
		return false;
	}
};
}  // namespace

char IRModelPass::ID = 0;
static RegisterPass<IRModelPass> X(
    "irmodel",			      // the option name -> -mba
    "LLVMIR Instruction modelling",   // option description
    true,			      // true as we don't modify the CFG
    true
);
