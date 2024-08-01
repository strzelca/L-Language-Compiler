#ifndef HANDLE_H
#define HANDLE_H

#include "builder.h"
#include "error_handler.h"
#include "parser.h"
#include <llvm/IR/IRBuilder.h>

static void HandleFunction() {
  if (auto FnAST = ParseDefinition()) {
    if (auto *FnIR = FnAST->codegen()) {
      fprintf(stderr, "Read function definition:\n");
      FnIR->print(errs());
      fprintf(stderr, "\n");
      ExitOnErr(jit->addModule(
          ThreadSafeModule(std::move(module), std::move(context))));
      InitializeModule();
    }
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

static void HandleExtern() {
  if (auto ProtoAST = ParseExtern()) {
    if (auto *FnIR = ProtoAST->codegen()) {
      fprintf(stderr, "Read extern: ");
      FnIR->print(errs());
      fprintf(stderr, "\n");
      functionProtos[ProtoAST->getId()] = std::move(ProtoAST);
    }
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

static void HandleTopLevelExpression() {
  if (auto FnAST = ParseTopLevelExpr()) {
    if (auto *FnIR = FnAST->codegen()) {
      auto rt = jit->getMainJITDylib().createResourceTracker();
      auto tsm = ThreadSafeModule(std::move(module), std::move(context));

      ExitOnErr(jit->addModule(std::move(tsm), rt));
      FnIR->print(errs());
      std::cout << "\n";
      InitializeModule();

      auto exprSymbol = ExitOnErr(jit->lookup("_main"));

      double (*fp)() = exprSymbol.getAddress().toPtr<double (*)()>();

      std::cout << "Evaluated to " << fp() << "\n";

      ExitOnErr(rt->remove());
    }
  } else {
    getNextToken();
  }
}

#endif