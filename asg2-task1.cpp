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

void ReportDeadFunctions();
void InvestigateFunction(const Function &F);

vector<Function*> mainList;
StringMap<StringRef> programFuncMap;
StringMap<Function*> definedFuncMap;

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
		
		// Find main, defined functions and populate maps
		// mainList holds a list of Function pointers to main() functions
		// programFuncMap maps key (functionName) to DEAD or ALIVE status
		// definedFuncMap maps key (functionName) to Function pointers
		Module::FunctionListType &functions = M->getFunctionList();
		for (Module::FunctionListType::iterator it = functions.begin(), it_end = functions.end(); it != it_end; ++it) {
			Function &func = *it;
			
			if (func.getName() == "main") {
				mainList.push_back(M->getFunction(func.getName()));
			}
		 	else if ((func.getName() != "main") && (func.isDeclaration() != 1) && (programFuncMap.find(func.getName()) == programFuncMap.end())) {
				programFuncMap[func.getName()] = "DEAD";
				definedFuncMap[func.getName()] = M->getFunction(func.getName());
			}
		}
	}
	
	// Step (2) Look through all main function and recurse investigate each of them
	for(auto &F : mainList) {
		for (auto &BB: *F) {
			for (auto &I : BB) {
				CallInst *Call = dyn_cast<CallInst>(&I);
				if (Call == nullptr) continue;
				Function *G = Call->getCalledFunction();
				if (G == nullptr) continue;
				programFuncMap[G->getName()] = "LIVE";
				if (definedFuncMap[G->getName()] != 0) {
					InvestigateFunction(*(definedFuncMap[G->getName()]));
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
				if (definedFuncMap[G->getName()] != 0) { 
					InvestigateFunction(*(definedFuncMap[G->getName()]));
				}
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
