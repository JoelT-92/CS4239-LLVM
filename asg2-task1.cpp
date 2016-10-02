#define DEBUG_TYPE "dead-function"
#include "llvm/DebugInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
#include <iostream>
#include <string>

// Data structs
#include "llvm/ADT/StringMap.h"

using namespace llvm;
using namespace std;

void ReportDeadFunctions();
void InvestigateFunction(const Function &F);

StringMap<StringRef> programFuncMap;

int main(int argc, char **argv) {
	for (int i = 1; i < argc; i++) {
		// Step (1) Parse the given IR File
		LLVMContext &Context = getGlobalContext();
		SMDiagnostic Err;
		Module *M = ParseIRFile(argv[i], Err, Context);
		if (M == nullptr) {
			fprintf(stderr, "failed to read IR file %s\n", argv[1]);
			return 1;
		}
	
		// Step (2) Add all referenced functions in the C program
		// This will include all STL function calls reference in program
		Module::FunctionListType &functions = M->getFunctionList();
		for (Module::FunctionListType::iterator it = functions.begin(), it_end = functions.end(); it != it_end; ++it) {
		  Function &func = *it;
		  if (func.getName() != "main") programFuncMap[func.getName()] = "DEAD";
		}

		// Step (3) Traverse all instructions
		// Retrieve called functions
		for (auto &F : *M) {
			if (F.getName() == "main") {
				for (auto &BB: F) {
					for (auto &I: BB) {
						CallInst *Call = dyn_cast<CallInst>(&I);
						if (Call == nullptr) continue;
						Function *G = Call->getCalledFunction();
						if (G == nullptr) continue;
						programFuncMap[G->getName()] = "LIVE";
						InvestigateFunction(*G);
					}
				}
			}
		}
	}

	// Step (4) Report on uncalled functions
	ReportDeadFunctions();
}

void InvestigateFunction(const Function &F) {
	for (auto &bb : F) {
		for (auto &i : bb) {
			const CallInst *Call = dyn_cast<CallInst>(&i);
		    if (Call == nullptr) continue;
		    Function *G = Call->getCalledFunction();
		    if (G == nullptr) continue;
		    if (programFuncMap[G->getName()] != "LIVE") {
				programFuncMap[G->getName()] = "LIVE";
				InvestigateFunction(*G);
			}
			else {
				// Encountered a function that was already called.
				// Prevent cycle by continue;
				continue;
			}
		}
	}
}

void ReportDeadFunctions() {
	for (StringMap<StringRef>::const_iterator it = programFuncMap.begin(), it_end = programFuncMap.end(); it != it_end; ++it) {
	  if (it->getValue() == "DEAD") {
		  errs() << "WARNING: function <" << it->getKey() << "> is a DEAD function" << "\n";
	  }
	  else if (it->getValue() == "MAYBE DEAD") {
		  errs() << "WARNING: function <" << it->getKey() << "> might be a DEAD function" << "\n";
	  }
	}
}