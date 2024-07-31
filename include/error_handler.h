#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include "ast.h"
#include <cstdio>
#include <llvm/Support/Error.h>
#include <memory>

static llvm::ExitOnError ExitOnErr;

std::unique_ptr<ExprAST> LogError(const char *Str) {
  fprintf(stderr, "Error: %s\n", Str);
  return nullptr;
}
std::unique_ptr<PrototypeAST> LogErrorP(const char *Str) {
  LogError(Str);
  return nullptr;
}

llvm::Value *LogErrorV(const char *Str) {
  LogError(Str);
  return nullptr;
}

#endif