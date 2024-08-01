#ifndef BUILDER_H
#define BUILDER_H

#include "ast.h"
#include "error_handler.h"
#include "jit.h"
#include "op.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Instruction.h"
#include <iostream>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/StandardInstrumentations.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Scalar/Reassociate.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>
#include <map>
#include <memory>
#include <utility>
#include <vector>

using namespace llvm;

static std::unique_ptr<LLVMContext> context;
static std::unique_ptr<IRBuilder<>> builder;
static std::unique_ptr<Module> module;
static std::unique_ptr<LLanguageJIT> jit;
static std::map<std::string, Value *> NamedValues;
static std::map<std::string, std::unique_ptr<PrototypeAST>> functionProtos;

// OPTIMIZATION MANAGERS
static std::unique_ptr<FunctionPassManager> fpm;
static std::unique_ptr<LoopAnalysisManager> lam;
static std::unique_ptr<FunctionAnalysisManager> fam;
static std::unique_ptr<CGSCCAnalysisManager> cgam;
static std::unique_ptr<ModuleAnalysisManager> mam;
static std::unique_ptr<PassInstrumentationCallbacks> pic;
static std::unique_ptr<StandardInstrumentations> si;

static void InitializeModule() {
  PassBuilder pb;

  // Open a new context and module.
  context = std::make_unique<LLVMContext>();
  module = std::make_unique<Module>("L Language JIT Compiler", *context);
  module->setDataLayout(jit->getDataLayout());

  // Create a new builder for the module.
  builder = std::make_unique<IRBuilder<>>(*context);

  // Initialize Optimization
  fpm = std::make_unique<FunctionPassManager>();
  lam = std::make_unique<LoopAnalysisManager>();
  fam = std::make_unique<FunctionAnalysisManager>();
  cgam = std::make_unique<CGSCCAnalysisManager>();
  mam = std::make_unique<ModuleAnalysisManager>();
  pic = std::make_unique<PassInstrumentationCallbacks>();
  si = std::make_unique<StandardInstrumentations>(*context, true);

  si->registerCallbacks(*pic, mam.get());

  fpm->addPass(InstCombinePass());
  fpm->addPass(ReassociatePass());
  fpm->addPass(GVNPass());
  fpm->addPass(SimplifyCFGPass());

  pb.registerModuleAnalyses(*mam);
  pb.registerFunctionAnalyses(*fam);
  pb.crossRegisterProxies(*lam, *fam, *cgam, *mam);
}

Value *NumberExprAST::codegen() {
  return ConstantFP::get(*context, APFloat(val));
}

Value *VariableExprAST::codegen() {
  Value *V = NamedValues[Id];
  if (!V)
    return LogErrorV("Unknown variable name");
  return V;
}

Value *BinaryExprAST::codegen() {
  Value *L = LHS->codegen();
  Value *R = RHS->codegen();
  if (!L || !R)
    return nullptr;

  switch (Op) {
  case BOP::add:
    return builder->CreateFAdd(L, R, "addtmp");
  case BOP::sub:
    return builder->CreateFSub(L, R, "subtmp");
  case BOP::mul:
    return builder->CreateFMul(L, R, "multmp");
  case BOP::le:
    L = builder->CreateFCmpULT(L, R, "cmptmp");
    // Convert bool 0/1 to double 0.0 or 1.0
    return builder->CreateUIToFP(L, Type::getDoubleTy(*context), "booltmp");
  default:
    return LogErrorV("invalid binary operator");
  }
}

Function *getFunction(std::string Id) {
  if (auto *F = module->getFunction(Id))
    return F;

  auto FI = functionProtos.find(Id);
  if (FI != functionProtos.end())
    return FI->second->codegen();

  // If no existing prototype exists, return null.
  return nullptr;
}

Value *CallExprAST::codegen() {
  Function *CalleeF = getFunction(Callee);
  if (!CalleeF)
    return LogErrorV("Unknown function referenced");

  if (CalleeF->arg_size() != Args.size())
    return LogErrorV("Incorrect # arguments passed");

  std::vector<Value *> ArgsV;
  for (unsigned i = 0, e = Args.size(); i != e; ++i) {
    ArgsV.push_back(Args[i]->codegen());
    if (!ArgsV.back())
      return nullptr;
  }

  return builder->CreateCall(CalleeF, ArgsV, "calltmp");
}

Function *PrototypeAST::codegen() {
  std::vector<Type *> Doubles(Args.size(), Type::getDoubleTy(*context));
  FunctionType *FT =
      FunctionType::get(Type::getDoubleTy(*context), Doubles, false);

  Function *F =
      Function::Create(FT, Function::ExternalLinkage, Id, module.get());

  unsigned idx = 0;
  for (auto &arg : F->args())
    arg.setName(Args[idx++]);

  return F;
}

Function *FunctionAST::codegen() {
  auto &P = *Proto;
  functionProtos[Proto->getId()] = std::move(Proto);
  Function *func = getFunction(P.getId());

  if (!func)
    return nullptr;

  BasicBlock *BB = BasicBlock::Create(*context, "entry", func);
  builder->SetInsertPoint(BB);

  NamedValues.clear();
  for (auto &arg : func->args())
    NamedValues[std::string(arg.getName())] = &arg;

  if (Value *retVal = Body->codegen()) {
    builder->CreateRet(retVal);
    verifyFunction(*func);

    fpm->run(*func, *fam);

    return func;
  }

  func->eraseFromParent();
  return nullptr;
}

#endif