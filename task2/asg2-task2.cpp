//Authors : A0133662J CHEW YUNG CHUNG, A0125473H TAN QIU HAO, JOEL, A0126332R JULIANA SENG
//Email(respectively) : A0133662@U.NUS.EDU, A0125473@U.NUS.EDU, A0126332@U.NUS.EDU
#define DEBUG_TYPE "escape"
#include "llvm/DebugInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Constants.h"
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
            if (F.getName() != "main") {
                multimap<Value*,Value*> allocaMap;
                map<Value*,size_t> lineMap;
                size_t line = 1;
                for (auto &BB: F) { // For each basic block BB
                    for (auto &I: BB) { // For each instruction I
                        if (I.getOpcode() == Instruction::Alloca) {
                            Value *v = dyn_cast<Value>(&I);
                            allocaMap.insert(pair<Value*,Value*>(v, 0));
                        } else if (I.getOpcode() == Instruction::Load) {
                            LoadInst *R = dyn_cast<LoadInst>(&I);
                            Value *v = dyn_cast<Value>(&I);
                            Value *vv = R->getPointerOperand();
                            allocaMap.insert(pair<Value*,Value*>(v, vv));
                        } else if (I.getOpcode() == Instruction::GetElementPtr) {
                            GetElementPtrInst *R = dyn_cast<GetElementPtrInst>(&I);
                            Value *v = dyn_cast<Value>(&I);
                            Value *vv = R->getPointerOperand();
                            allocaMap.insert(pair<Value*,Value*>(v, vv));
                        } else if (I.getOpcode() == Instruction::Store) {
                            StoreInst *R = dyn_cast<StoreInst>(&I);
                            Value *v = R->getPointerOperand();
                            Value *vv = R->getValueOperand();
                            if (isa<ConstantExpr>(vv)) {
                                ConstantExpr *ce = dyn_cast<ConstantExpr>(vv);
                                if (isa<GlobalValue>(ce->getOperand(0))) {
                                    allocaMap.insert(pair<Value*,Value*>(v, vv));
                                    if (vv && allocaMap.find(vv) == allocaMap.end()) {
                                        allocaMap.insert(pair<Value*,Value*>(vv, 0));
                                    }

                                }
                            } else if (isa<Constant>(vv)) { continue; }
                            else {
                                allocaMap.insert(pair<Value*,Value*>(v, vv));
                                if (vv && allocaMap.find(vv) == allocaMap.end()) {
                                    allocaMap.insert(pair<Value*,Value*>(vv, v));
                                }
                            }
                        } else if (isa<UnaryInstruction>(&I)) {
                            Value *v = dyn_cast<Value>(&I);
                            Value *vv = I.getOperand(0);
                            allocaMap.insert(pair<Value*,Value*>(v, vv));
                        } else if (I.getOpcode() == Instruction::Ret) {    //for the program, its because the alloca is surprisingly at a higher address than the rest
                            ReturnInst *R = dyn_cast<ReturnInst>(&I);
                            if (R) {
                                Value *v = R->getReturnValue();
                                if (v && allocaMap.size() > 0) {
                                    pair<multimap<Value*,Value*>::iterator,multimap<Value*,Value*>::iterator> ret;
                                    Value *nextV = 0, *prevV = dyn_cast<Value>(R), *first;
                                    while (true) {
                                        ret = allocaMap.equal_range(v);
                                        for (multimap<Value*,Value*>::iterator it = ret.first; it != ret.second; ++it) {
                                            if (isa<AllocaInst>(v)) {
                                                if (lineMap[it->second] < lineMap[prevV]) {
                                                    nextV = it->second;
                                                }
                                            } else {
                                                if (lineMap[it->second] <= lineMap[v]) {
                                                    nextV = it->second;
                                                    prevV = v;
                                                }
                                            }
                                        }
                                        if (nextV == 0 || v == nextV) {                                            
                                            if (v && v->getName() != "") {
                                                errs() << v->getName() << " escaped \n";
                                            }
                                            break;
                                        }
                                        v = nextV;
                                    }
                                }
                            }
                        }
                        Value *v = dyn_cast<Value>(&I);
                        lineMap[v] = line;
                        line++;
                    }
                }
            }
        }
    }
}
