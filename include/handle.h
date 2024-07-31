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
    }
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

static void HandleTopLevelExpression() {
  if (auto FnAST = ParseTopLevelExpr()) {
    if (FnAST->codegen()) {
      auto rt = jit->getMainJITDylib().createResourceTracker();
      auto tsm = ThreadSafeModule(std::move(module), std::move(context));

      ExitOnErr(jit->addModule(std::move(tsm), rt));
      InitializeModule();

      auto exprSymbol = ExitOnErr(jit->lookup(FnAST->getId()));

      // double (*fp)() = exprSymbol.getAddress().toPtr<double (*)()>();

      // fprintf(stderr, "Evaluated to %f\n", fp());

      ExitOnErr(rt->remove());
    }
  } else {
    getNextToken();
  }
}

#endif