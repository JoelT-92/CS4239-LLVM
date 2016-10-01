#define DEBUG_TYPE "dead-function"
#include "llvm/DebugInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include <map>
#include <iostream>
#include <string>

using namespace llvm;

int main(int argc, char **argv) {
	// Step (1) Parse the given IR File
	LLVMContext &Context = getGlobalContext();
	SMDiagnostic Err;
	Module *M = ParseIRFile(argv[1], Err, Context);
	if (M == nullptr) {
		fprintf(stderr, "failed to read IR file %s\n", argv[1]);
		return 1;
	}

	// Step (2) Initialize an array of all functions
	std::map<std::string, int> function_array;
	for (auto &F : *M) {
		std::string function_name ((&F)->getName().str().c_str());
		//MDNode *n = (&F)->getMetadata("dbg"); // unable to retrieve line number
		function_array[function_name] = 0;
		//std::cout << "Function Name = " << function_name << std::endl;
	}

	// Step (3) Traverse all instructions
	// Retrieve called functions
	for (auto &F : *M) {
		for (auto &BB: F) {
			for (auto &I: BB) {
				CallInst *Call = dyn_cast<CallInst>(&I);
				if (Call == nullptr) continue;
				Function *G = Call->getCalledFunction();
				if (G == nullptr) continue;
				std::string called_function (G->getName().str().c_str());
				function_array[called_function]++;
				//std::cout << "Called function = " << called_function << std::endl;
			}
		}
	}

	// Step (4) Report on uncalled functions
	for(auto &function : function_array) {
		if ((function.second == 0) && (function.first.compare("main") != 0)) {
			std::cout << "Dead function detected: " << function.first << std::endl;
		}
	}
}
