#define DEBUG_TYPE "dead-function"
#include "llvm/DebugInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CallSite.h"
#include "llvm/ADT/StringMap.h"

#include <iostream>
#include <string>
#include <vector>

using namespace llvm;
using namespace std;

void ReportDeadFunctions();
void InvestigateFunction(Function &F);
Function* ResolveIndirectCall(CallSite &CS);
void InvestigateStoreInst(StoreInst *S);

vector<Function*> mainList;
StringMap<StringRef> programFuncMap;
StringMap<Function*> definedFuncMap;
StringMap<Function*> pointerFuncMap;

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
        // pointerFuncMap maps key (variableName) to Function pointers
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
                StoreInst *S = dyn_cast<StoreInst>(&I);
                if (S) InvestigateStoreInst(S);
                CallSite Call(&I);
                if (!Call) continue;
                Function *G = Call.getCalledFunction();
                if (G == nullptr) {
                    G = ResolveIndirectCall(Call);
                    if (G == nullptr) continue;
                }
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

void InvestigateFunction(Function &F) {
	for (auto &bb : F) {
		for (auto &i : bb) {
			StoreInst *S = dyn_cast<StoreInst>(&i);
			if (S) InvestigateStoreInst(S);
			CallSite Call(&i);
			if (!Call) continue;
			Function *G = Call.getCalledFunction();
			if (G == nullptr) {
				G = ResolveIndirectCall(Call);
				if (G == nullptr) continue;
			}
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

Function* ResolveIndirectCall(CallSite &CS) {
    Value *v = CS.getCalledValue();
    GlobalAlias *ga = cast<GlobalAlias>(v);
    if (!ga) return nullptr;
    Constant *c = ga->getAliasee();
    if (!c) return nullptr;
    c = cast<Constant>(c->stripPointerCasts());
	while (!isa<AllocaInst>(c) && !isa<GlobalValue>(c)) {
		ga = cast<GlobalAlias>(c);
		if (!ga) return nullptr;
		c = ga->getAliasee();
		if (!c) return nullptr;
		c = cast<Constant>(c->stripPointerCasts());
	}
    StringMap<Function*>::iterator sm = pointerFuncMap.find(c->getName());
    if (sm != pointerFuncMap.end()) {
        return sm->second;
    } else {
        return nullptr;
    }
}

void InvestigateStoreInst(StoreInst *S) {
    Value *v = S->getValueOperand();
    if (!v) return;
    v = v->stripPointerCasts();
    if (!v) return;
    GlobalAlias *ga = cast<GlobalAlias>(S);
    if (!ga) return;
    Constant *c = ga->getAliasee();
    if (!c) return;
    c = cast<Constant>(c->stripPointerCasts());
    if (isa<Function>(v)) {
        pointerFuncMap[c->getName()] = dyn_cast<Function>(v);
    } else {
        Value *v2 = S->getPointerOperand();
        Type* ptrType = v2->getType()->getPointerElementType();
        if (PointerType* pt = dyn_cast<PointerType>(ptrType))
        {
            do {
                Type* pointedType = pt->getTypeAtIndex((unsigned int)0);
                if (pointedType->isFunctionTy())
                {
                    GlobalAlias *gga = cast<GlobalAlias>(v);
                    if (gga) {
						StringRef name;
						if (isa<Argument>(gga))	name = gga->getName();
						else name = gga->getAliasee()->getName();
						if (pointerFuncMap.find(name) != pointerFuncMap.end()) {
							pointerFuncMap[c->getName()] = pointerFuncMap[name];
						} else {
							pointerFuncMap[c->getName()] = nullptr;
						}
                    }
                    break;
                }
                ptrType = pointedType;
            } while ((pt = dyn_cast<PointerType>(ptrType)));
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
