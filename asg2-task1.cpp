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
	std::map<std::string, int> function_array;
	for (int i = 1; i < argc; i++) {
		// Step (1) Parse the given IR File
		LLVMContext &Context = getGlobalContext();
		SMDiagnostic Err;
		Module *M = ParseIRFile(argv[i], Err, Context);
		if (M == nullptr) {
			fprintf(stderr, "failed to read IR file %s\n", argv[1]);
			return 1;
		}

		// Step (2) Add all functions to array
		for (auto &F : *M) {
			std::string function_name ((&F)->getName().str().c_str());
			//MDNode *n = (&F)->getMetadata("dbg"); // unable to retrieve line number
			if (function_array.find(function_name) == function_array.end()) {
	  			function_array[function_name] = 0; // add to map if it does not exist
			}
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
					if (function_array.find(called_function) == function_array.end()) {
			  			function_array[called_function] = 0; // add to map if it does not exist
					}
					function_array[called_function]++;
					//std::cout << "Called function = " << called_function << std::endl;
				}
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
