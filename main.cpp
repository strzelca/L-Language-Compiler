#include "handle.h"
#include "op.h"
#include "tok.h"
#include "token.h"
#include <cstdio>

static void mainLoop() {
  while (true) {
    fprintf(stderr, "ready> ");
    switch (curTok) {
    case tok_eof:
      return;
    case ';':
      getNextToken();
    case tok_fun:
      HandleFunction();
    default:
      HandleTopLevelExpression();
    }
  }
}

int main(int argc, char *argv[]) {

  BopPrecedence[BOP::le] = 10;
  BopPrecedence[BOP::ge] = 10;
  BopPrecedence[BOP::add] = 20;
  BopPrecedence[BOP::sub] = 20;
  BopPrecedence[BOP::mul] = 30;
  BopPrecedence[BOP::div] = 30;
  BopPrecedence[BOP::mod] = 40;

  fprintf(stderr, "ready> ");
  getNextToken();

  mainLoop();

  return 0;
}