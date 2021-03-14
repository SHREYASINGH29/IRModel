#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include <iostream>
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"

using namespace llvm;

namespace {

#define DEBUG_TYPE "irmodel"                                                   

class IRModelPass : public FunctionPass {
       public:
	static char ID;
	IRModelPass() : FunctionPass(ID) {}
	void handleAlloca(Instruction &I){
		if(auto *AI = dyn_cast<AllocaInst>(&I)){
			std::cout<<AI->getName().str()<< " = new memloc"<<std::endl;
		}
	}
	bool runOnFunction(Function &F) override {
		for(inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I){
			handleAlloca(*I);
		}
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
