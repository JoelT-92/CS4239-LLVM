#define DEBUG_TYPE "bad-cast"
#include "llvm/DebugInfo.h"
#include "llvm/Pass.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/CallSite.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/Support/CFG.h"

//todo : change stringref from name to function.

using namespace llvm;

int main(int argc, char **argv) {
	std::vector<Module*> ModuleSet;
	for (int i = 1; i < argc; i++) {
		// Step (1) Parse the given IR File
		LLVMContext &Context = getGlobalContext();
		SMDiagnostic Err;
		Module *M = ParseIRFile(argv[i], Err, Context);
		if (M == nullptr) {
			fprintf(stderr, "failed to read IR file %s\n", argv[1]);
			return 1;
		}
		ModuleSet.push_back(M);
	}

	// Step (2) Traverse functions for call tree
	for (auto &M : ModuleSet) {
		for (auto &F : *M) {
			for (auto &BB : F) {
				errs() << F.getName() << "\n";
				for (auto &I : BB) {
					AllocaInst *A = dyn_cast<AllocaInst>(&I);
					if (A) {
						for (Value::use_iterator i = A->use_begin(), e = A->use_end(); i != e; ++i) {
							if (isa<StoreInst>(*i)){}//keep track of what is stored
							if (isa<ReturnInst>(*i)) {}//keep track
							errs() << **i << "\n";
						}
					} else if (isa<ReturnInst>(&I)) {
						ReturnInst *R = dyn_cast<ReturnInst>(&I);
						Value *V = R->getReturnValue();
						if (V) {
							errs() << "ret val " << *V << "\n";
						}
					}
				}
			}
		}
	}
	return 0;
}
