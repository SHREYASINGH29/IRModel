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
	std::string getConst(Value *Val){
		if(auto *CI = dyn_cast<ConstantInt>(Val)){
			return CI->getValue().toString(10, true);
		}else{
			return Val->getName().str();
		}
	}
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
	void handleBinaryOps(Instruction *BI){
		Value *BO1 = BI->getOperand(0);
		Value *BO2 = BI->getOperand(1);
		char Op;
		if(BI->getOpcode()== Instruction::Add){
			Op = '+';
		}else if(BI->getOpcode()== Instruction::Mul){
			Op = '*';
		}else if(BI->getOpcode()== Instruction::Sub){
			Op = '-';
		}else if(BI->getOpcode()==Instruction::UDiv){
			Op = '/';
		}
		std::cout<<BI->getName().str()<<" = "<<getConst(BO1)<<" "<<Op<<" "<<getConst(BO2)<<std::endl;
	}
	void handleBitCast(BitCastInst *BCI){
		std::string Ref = BCI->getName().str();
		Value *Dest = BCI->getOperand(0);
		std::cout<<Ref<<" = "<<getConst(Dest)<<std::endl;
	}
	void handleSextCast(SExtInst *SEI){
		std::string Def = SEI->getName().str();
		Value *Dest = SEI->getOperand(0);
		std::cout<<Def<<" = "<<getConst(Dest)<<std::endl;
	}
	void handleCall(CallInst *CI){
		if(!(CI->doesNotReturn())){
			std::cout<<CI->getName().str()<<" = ";
		}
		Function *CF = CI->getCalledFunction();
		if(CF!=nullptr){
			std::cout<<CF->getName().str();
		}
		std::cout<<"( ";
		unsigned Num = CI->getNumArgOperands();
		for(int i = 0; i < Num;i++){
			std::cout<<getConst(CI->getArgOperand(i))<<" ";
		}
		std::cout<<" )"<<std::endl;
	}
	void handlePointer(GetElementPtrInst *GP){
		std::cout << GP->getName().str() << " = &";
		std::cout << GP->getPointerOperand()->getName().str();
		std::string StrIdx = "[";
		for(Value* Idx: GP->indices()){
			StrIdx += (getConst(Idx) + "][");
		}
		StrIdx += "]";
		StrIdx = StrIdx.substr(0, StrIdx.size() - 2);
		std::cout << StrIdx << "\n";
	}
	bool runOnFunction(Function &F) override {
		for(inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I){
			errs()<<*I<<"\n";
			if(auto *AI = dyn_cast<AllocaInst>(&*I)){
				handleAlloca(AI);
			}else if(auto *LI = dyn_cast<LoadInst>(&*I)){
				handleLoad(LI);
			}else if(auto *SI = dyn_cast<StoreInst>(&*I)){
				handleStore(SI);
			}else if(auto *RI = dyn_cast<ReturnInst>(&*I)){
				handleReturn(RI);
			}else if((*I).isBinaryOp()){
				handleBinaryOps(&*I);
			}else if(auto *BCI = dyn_cast<BitCastInst>(&*I)){
				handleBitCast(BCI);
			}else if(auto *SEI = dyn_cast<SExtInst>(&*I)){
				handleSextCast(SEI);
			}else if(auto *CI = dyn_cast<CallInst>(&*I)){
				handleCall(CI);
			}else if(auto *GP = dyn_cast<GetElementPtrInst>(&*I)){
				handlePointer(GP);
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
