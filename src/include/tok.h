#ifndef TOK_H
#define TOK_H

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <store.h>
#include <token.h>

static int curTok;

static int gettok() {
  static int lastChar = ' ';

  while (isspace(lastChar))
    lastChar = getchar();

  if (isalpha(lastChar)) {
    IdStr = lastChar;

    while (isalnum(lastChar = getchar()))
      IdStr += lastChar;

    if (IdStr == "if")
      return tok_if;
    if (IdStr == "while")
      return tok_while;
    if (IdStr == "const")
      return tok_const;
    if (IdStr == "var")
      return tok_var;
    if (IdStr == "function")
      return tok_fun;

    return tok_id;
  }

  if (isdigit(lastChar) || lastChar == '.') {
    do {
      ValStr += lastChar;
      lastChar = getchar();
    } while (isdigit(lastChar) || lastChar == '.');

    ValNum = strtod(ValStr.c_str(), 0);
    return tok_val;
  }

  if (lastChar == '#') {
    do
      lastChar = getchar();
    while (lastChar != EOF && lastChar != '\n' && lastChar != '\r');

    if (lastChar != EOF)
      return gettok();
  }

  if (lastChar == EOF)
    return tok_eof;

  int thisChar = lastChar;
  lastChar = getchar();
  return thisChar;
}

static int getNextToken() { return curTok = gettok(); }

#endif