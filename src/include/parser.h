#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "error_handler.h"
#include "op.h"
#include "tok.h"
#include "token.h"
#include <algorithm>
#include <memory>
#include <store.h>

static std::unique_ptr<ExprAST> ParsePrimary();

// E ::= E bop E
static std::unique_ptr<ExprAST> ParseBinOpRHS(int exprPrec,
                                              std::unique_ptr<ExprAST> LHS) {
  while (true) {
    int tokPrec = getTokPrecedence();
    if (tokPrec < exprPrec)
      return LHS;

    int binOp = curTok;
    getNextToken();

    auto RHS = ParsePrimary();
    if (!RHS)
      return nullptr;

    int nextPrec = getTokPrecedence();
    if (tokPrec < nextPrec) {
      RHS = ParseBinOpRHS(tokPrec + 1, std::move(RHS));
      if (!RHS)
        return nullptr;
    }

    LHS =
        std::make_unique<BinaryExprAST>(binOp, std::move(LHS), std::move(RHS));
  }
}

static std::unique_ptr<ExprAST> ParseExpression() {
  auto LHS = ParsePrimary();
  if (!LHS)
    return nullptr;

  return ParseBinOpRHS(0, std::move(LHS));
}

// E ::= v
static std::unique_ptr<ExprAST> ParseNumberExpr() {
  auto result = std::make_unique<NumberExprAST>(ValNum);
  getNextToken();
  return std::move(result);
}

// E ::= (E)
static std::unique_ptr<ExprAST> ParseParenExpr() {
  getNextToken();
  auto V = ParseExpression();

  if (!V)
    return nullptr;

  if (curTok != ')')
    return LogError("expected )");
  getNextToken();
  return V;
}

// E ::= Id | Id(ae)
static std::unique_ptr<ExprAST> ParseIdentifierExpr() {
  std::string Id = IdStr;

  getNextToken();

  if (curTok != '(')
    return std::make_unique<VariableExprAST>(Id);

  // Call
  getNextToken();
  std::vector<std::unique_ptr<ExprAST>> Args;
  if (curTok != ')') {
    while (true) {
      if (auto arg = ParseExpression())
        Args.push_back(std::move(arg));
      else
        return nullptr;

      if (curTok == ')')
        break;

      if (curTok != ',')
        return LogError("Expected ')' or  ',' in argument list");
      getNextToken();
    }
  }

  getNextToken();

  return std::make_unique<CallExprAST>(Id, std::move(Args));
}

// PROG ::= E
static std::unique_ptr<ExprAST> ParsePrimary() {
  switch (curTok) {
  default:
    return LogError("Unknown token while expecting expression");
  case tok_id:
    ParseIdentifierExpr();
  case tok_val:
    ParseNumberExpr();
  case '(':
    ParseParenExpr();
  }
}

// D ::= function Id(form) -> T {C; return E}
static std::unique_ptr<PrototypeAST> ParsePrototype() {
  if (curTok != tok_id)
    return LogErrorP("Expected function name in prototype");

  std::string Id = IdStr;
  getNextToken();

  if (curTok != '(')
    return LogErrorP("Expected '(' in prototype");

  // Read the list of argument names.
  std::vector<std::string> ArgNames;
  while (getNextToken() == tok_id)
    ArgNames.push_back(IdStr);
  if (curTok != ')')
    return LogErrorP("Expected ')' in prototype");

  // success.
  getNextToken(); // eat ')'.

  return std::make_unique<PrototypeAST>(Id, std::move(ArgNames));
}

static std::unique_ptr<FunctionAST> ParseDefinition() {
  getNextToken();
  auto proto = ParsePrototype();
  if (!proto)
    return nullptr;

  if (auto E = ParseExpression())
    return std::make_unique<FunctionAST>(std::move(proto), std::move(E));
  return nullptr;
}

static std::unique_ptr<FunctionAST> ParseTopLevelExpr() {
  if (auto E = ParseExpression()) {
    // Make an anonymous proto.
    auto Proto = std::make_unique<PrototypeAST>("", std::vector<std::string>());
    return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
  }
  return nullptr;
}

#endif