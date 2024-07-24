#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <ast.h>
#include <cstdio>
#include <memory>

std::unique_ptr<ExprAST> LogError(const char *Str) {
  fprintf(stderr, "Error: %s\n", Str);
  return nullptr;
}
std::unique_ptr<PrototypeAST> LogErrorP(const char *Str) {
  LogError(Str);
  return nullptr;
}

#endif