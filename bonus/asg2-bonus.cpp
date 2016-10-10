#define DEBUG_TYPE "bonus"
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

StringMap<StringRef> globalPtrVarMap;

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
        
        // Keep track of all global pointers in globalPtrVarMap
        Module::GlobalListType &globals = M->getGlobalList();
        for (Module::GlobalListType::iterator it = globals.begin(), it_end = globals.end(); it != it_end; ++it) {
            GlobalVariable &globalVar = *it;            
            Value *castGlobalValue = dyn_cast<Value>(&globalVar);
            if (castGlobalValue != nullptr) {
                PointerType *globalVarPtr = dyn_cast<PointerType>(castGlobalValue->getType()->getPointerElementType());
                if (globalVarPtr != nullptr) {
                    globalPtrVarMap[globalVar.getName()] = "";
                } 
            } 
        }
    
        // Step (2) Traverse all instructions
        for (auto &F: *M) { // For each function F
            if (F.getName() != "main") {
                StringMap<StringRef> allocaMap;
                for (auto &BB: F) { // For each basic block BB
                    for (auto &I: BB) { // For each instruction I
                        if (I.getOpcode() == Instruction::Alloca) {
                            AllocaInst *allocInstr = dyn_cast<AllocaInst>(&I);
                            allocaMap[allocInstr->getName()] = "";
                        }
                        
                        if (I.getOpcode() == Instruction::Store) {
                            StringRef srcValue = I.getOperand(0)->getName();
                            StringRef dstValue = I.getOperand(1)->getName();
                            
                            if ((allocaMap.find(srcValue) != allocaMap.end()) && (globalPtrVarMap.find(dstValue) != globalPtrVarMap.end())) {
                                errs() << "WARNING: pointer <" << srcValue << "> in the function <" << (&F)->getName() << "> tries to escape through the global pointer <" << dstValue << ">!" << "\n";
                            } 
                        }

                        if (I.getOpcode() == Instruction::Ret) {
                            ReturnInst *R = dyn_cast<ReturnInst>(&I);
                            if (!R) continue;
                            Value *v = R->getReturnValue();
                            if (!v) continue;
                            GlobalAlias *ga = cast<GlobalAlias>(v);
                            if (!ga) continue;
                            Constant *c = ga->getAliasee();
                            if (!c) continue;
                            c = cast<Constant>(c->stripPointerCasts());
                            while (!isa<AllocaInst>(c) && !isa<GlobalValue>(c)) {
                                ga = cast<GlobalAlias>(c);
                                if (!ga) goto end;
                                c = ga->getAliasee();
                                if (!c) goto end;
                                c = cast<Constant>(c->stripPointerCasts());
                            }
                            if (R->getReturnValue() != nullptr) {
                                if (allocaMap.find(c->getName()) != allocaMap.end()) {
                                    errs() << "WARNING: variable <" << c->getName() << "> in the function <" << (&F)->getName().str() << "> will not exist after the return" << "\n";
                                }
                            }
                            end: continue;
                        }
                    }
                }
            }
        }
    }
}
