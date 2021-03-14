#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include <iostream>
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"

using namespace llvm;

namespace {

#define DEBUG_TYPE "irmodel"                                                   

class IRModelPass : public FunctionPass {
       public:
	static char ID;
	IRModelPass() : FunctionPass(ID) {}
	void handleAlloca(AllocaInst *AI){
		std::cout<<AI->getName().str()<< " = new memloc"<<std::endl;
	}
	void handleLoad(LoadInst *LI){
		std::cout<<LI->getName().str()<<" = *"<<LI->getPointerOperand()->getName().str()<<std::endl;
	}
	void handleStore(StoreInst *SI){
		std::cout << "*" << SI->getPointerOperand()->getName().str() << " = ";
		Value * ValueOp = SI->getValueOperand();
		if(auto *CI = dyn_cast<ConstantInt>(ValueOp)){
			std::cout << CI->getValue().toString(10, true);
		} else {
			std::cout << ValueOp->getName().str();
		}
		std::cout << std::endl;
	}
	void handleReturn(ReturnInst *RI){
		Value *RetVal = RI->getReturnValue();
		if(!RetVal){
			std::cout<<"return void"<<std::endl;
		}else if(auto *RC = dyn_cast<ConstantInt>(RetVal)){
			std::cout<<"return "<<RC->getValue().toString(10,true)<<std::endl;
		}else{
			std::cout<<"return "<<RetVal->getName().str()<<std::endl;
		}
	}
	bool runOnFunction(Function &F) override {
		for(inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I){
			if(auto *AI = dyn_cast<AllocaInst>(&*I)){
				handleAlloca(AI);
			}else if(auto *LI = dyn_cast<LoadInst>(&*I)){
				handleLoad(LI);
			}else if(auto *SI = dyn_cast<StoreInst>(&*I)){
				handleStore(SI);
			}else if(auto *RI = dyn_cast<ReturnInst>(&*I)){
				handleReturn(RI);
			}
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
