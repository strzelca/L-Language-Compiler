#ifndef TOK_H
#define TOK_H

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <store.h>
#include <token.h>

static int curTok;
static FILE *src;

static int gettok() {
  static int lastChar = ' ';

  while (isspace(lastChar))
    lastChar = getc(src);

  if (isalpha(lastChar)) {
    IdStr = lastChar;

    while (isalnum(lastChar = getc(src)))
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
    ValStr = "";
    do {
      ValStr += lastChar;
      lastChar = getc(src);
    } while (isdigit(lastChar) || lastChar == '.');

    ValNum = strtod(ValStr.c_str(), 0);
    return tok_val;
  }

  if (lastChar == '#') {
    do
      lastChar = getc(src);
    while (lastChar != EOF && lastChar != '\n' && lastChar != '\r');

    if (lastChar != EOF)
      return gettok();
  }

  if (lastChar == EOF)
    return tok_eof;

  int thisChar = lastChar;
  lastChar = getc(src);
  return thisChar;
}

static int getNextToken() { return curTok = gettok(); }

#endif