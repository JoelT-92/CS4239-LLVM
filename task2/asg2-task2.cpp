#define DEBUG_TYPE "dead-function"
#include "llvm/DebugInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/StringMap.h"

#include <iostream>
#include <string>
#include <vector>

using namespace llvm;
using namespace std;

int main(int argc, char **argv) {
	
	// Step (1) Parse all IR files. 
	// Locate all main functions and store them in mainList
	// Assume that each main include all other files
	for (int i = 1; i < argc; i++) {
		LLVMContext &Context = getGlobalContext();
		SMDiagnostic Err;
		Module *M = ParseIRFile(argv[i], Err, Context);
		if (M == nullptr) {
			fprintf(stderr, "failed to read IR file %s\n", argv[1]);
			return 1;
		}
	
		// Step (2) Traverse all instructions
		for (auto &F: *M) { // For each function F
			StringMap<StringRef> allocaMap;
			StringRef latestLoadValue;
			//cout << "Function name:" << (&F)->getName().str() << endl;
			for (auto &BB: F) { // For each basic block BB
				for (auto &I: BB) { // For each instruction I
					if (I.getOpcode() == Instruction::Alloca) {
						AllocaInst *allocInstr = dyn_cast<AllocaInst>(&I);
						allocaMap[allocInstr->getName()] = "";
						//cout << "New pointer name:" << allocInstr->getName().str() << "\n";
					}

					if (I.getOpcode() == Instruction::Load) {
						LoadInst *loadInstr = dyn_cast<LoadInst>(&I);
						latestLoadValue = loadInstr->getPointerOperand()->getName();
						//cout << "Load name:" << loadInstr->getPointerOperand()->getName().str() << "\n";
					}

					if (I.getOpcode() == Instruction::Ret) {
						ReturnInst *returnInstr = dyn_cast<ReturnInst>(&I);
						if (returnInstr->getReturnValue() != nullptr) {
							if (allocaMap.find(latestLoadValue) != allocaMap.end()) {
								errs() << "WARNING: pointer <" << latestLoadValue.str() << "> in the function <" << (&F)->getName().str() << "> will not exist after the return" << "\n";
							}
						}
					}
				}
			}
		}
	}
}
