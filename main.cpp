#include "builder.h"
#include "handle.h"
#include "jit.h"
#include "lexer.h"
#include "op.h"
#include "token.h"
#include <cstdio>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <memory>

static void mainLoop() {
  while (true) {
    switch (curTok) {
    case tok_eof:
      return;
    case ';':
      getNextToken();
      break;
    case tok_fun:
      HandleFunction();
      break;
    default:
      HandleTopLevelExpression();
      break;
    }
  }
}

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

/// putchard - putchar that takes a double and returns 0.
extern "C" DLLEXPORT double putchard(double X) {
  fputc((char)X, stderr);
  return 0;
}

/// printd - printf that takes a double prints it as "%f\n", returning 0.
extern "C" DLLEXPORT double printd(double X) {
  fprintf(stderr, "%f\n", X);
  return 0;
}

int main(int argc, char *argv[]) {
  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  InitializeNativeTargetAsmParser();

  BopPrecedence[BOP::le] = 10;
  BopPrecedence[BOP::ge] = 10;
  BopPrecedence[BOP::add] = 20;
  BopPrecedence[BOP::sub] = 20;
  BopPrecedence[BOP::mul] = 30;
  BopPrecedence[BOP::div] = 30;
  BopPrecedence[BOP::mod] = 40;

  if (argc > 1)
    src = fopen(argv[1], "r");
  else {
    fprintf(stderr, "Source File is Required");
    return -1;
  }

  getNextToken();

  jit = ExitOnErr(LLanguageJIT::Create());

  InitializeModule();

  mainLoop();

  module->print(errs(), nullptr);

  return 0;
}