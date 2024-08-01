#ifndef OP_H
#define OP_H

#include "lexer.h"
#include <map>
enum BOP {
  add = '+',
  sub = '-',
  mul = '*',
  div = '/',
  mod = '%',
  le = '<',
  ge = '>'
};

enum UOP { neg = '!' };

static std::map<char, int> BopPrecedence;

static int getTokPrecedence() {
  if (!isascii(curTok))
    return -1;

  int tokPrec = BopPrecedence[curTok];
  if (tokPrec <= 0)
    return -1;
  return tokPrec;
}

#endif