#define DEBUG_TYPE "bad-cast"
#include "llvm/DebugInfo.h"
#include "llvm/Pass.h"
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

//todo : change stringref from name to function. change function to null when set to constants
//fix the localFUncPtrNonsense calls function 1

using namespace llvm;

int main(int argc, char **argv) {
	std::vector<Module*> ModuleSet;
	std::vector<const User **> FunctionSet;
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
	SmallPtrSet<BasicBlock*, 8> Reachable;
	for (auto &M : ModuleSet) {
		for (auto &F : *M) {
			for (auto &BB : F) {
				StringMap<StringRef> pointerFuncMap;
				for (auto &I : BB) {
					if (isa<StoreInst>(&I)) {
						StoreInst *s = dyn_cast<StoreInst>(&I);
						Value *v = s->getValueOperand();
						if (!v) {
							continue;
						}
						v = v->stripPointerCasts();
						if (v) {
							GlobalAlias *ga = cast<GlobalAlias>(s);
							if (!ga) {
								continue;
							}
							Constant *c = ga->getAliasee();
							if (!c) {
								continue;
							}
							if (isa<Function>(v)) {
								pointerFuncMap[c->getName()] = v->getName();
							} else {
								Value *v2 = s->getPointerOperand();
								Type* ptrType = v2->getType()->getPointerElementType();
								if (PointerType* pt = dyn_cast<PointerType>(ptrType))
								{
									do {
										Type* pointedType = pt->getTypeAtIndex((unsigned int)0);
										if (pointedType->isFunctionTy())
										{
											GlobalAlias *gga = cast<GlobalAlias>(v);
											if (gga) {
												if (pointerFuncMap.find(gga->getAliasee()->getName()) != pointerFuncMap.end()) {
													pointerFuncMap[c->getName()] = pointerFuncMap[gga->getAliasee()->getName()];
												}
											} else {
												pointerFuncMap[c->getName()] = "";
											}
											break;
										}
										ptrType = pointedType;
									} while (pt = dyn_cast<PointerType>(ptrType));
								}
							}
						}
					} else {
						CallInst *Call = dyn_cast<CallInst>(&I);
						if (Call == nullptr) continue;
						Function *G = Call->getCalledFunction();
						if (G != nullptr) {
							if (!G->isDeclaration()) {
								errs() << "Instruction calls " << G->getName() << "\n";
							}
						} else {
							Value *v = Call->getCalledValue();
							GlobalAlias *ga = cast<GlobalAlias>(v);
							if (!ga) {
								continue;
							}
							Constant *c = ga->getAliasee();
							if (!c) {
								continue;
							}
							StringMap<StringRef>::iterator sm = pointerFuncMap.find(c->getName());
							if (sm != pointerFuncMap.end()) {
								errs() << c->getName() << " calls " << sm->second << "\n";
							} else {
								errs() << "unknown call\n";
							}
						}
					}
				}
			}
		}
	}
	return 0;
}
