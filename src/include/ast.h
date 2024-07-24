#ifndef AST_H
#define AST_H

#include <memory>
#include <string>
#include <utility>
#include <vector>
class ExprAST {
public:
  virtual ~ExprAST() = default;
};

class NumberExprAST : public ExprAST {
  double val;

public:
  NumberExprAST(double val) : val(val) {}
};

class VariableExprAST : public ExprAST {
  std::string Id;

public:
  VariableExprAST(const std::string &Id) : Id(Id) {}
};

class BinaryExprAST : public ExprAST {
  char Op;
  std::unique_ptr<ExprAST> LHS, RHS;

public:
  BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
                std::unique_ptr<ExprAST> RHS)
      : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

class UnaryExprAST : public ExprAST {
  char Op;
  std::unique_ptr<ExprAST> HS;

public:
  UnaryExprAST(char Op, std::unique_ptr<ExprAST> HS)
      : Op(Op), HS(std::move(HS)) {}
};

class CallExprAST : public ExprAST {
  std::string Callee;
  std::vector<std::unique_ptr<ExprAST>> Args;

public:
  CallExprAST(const std::string &Callee,
              std::vector<std::unique_ptr<ExprAST>> Args)
      : Callee(Callee), Args(std::move(Args)) {}
};

class PrototypeAST {
  std::string Id;
  std::vector<std::string> Args;

public:
  PrototypeAST(const std::string &Id, std::vector<std::string> Args)
      : Id(Id), Args(std::move(Args)) {}
};

class FunctionAST {
  std::unique_ptr<PrototypeAST> Proto;
  std::unique_ptr<ExprAST> Body;

public:
  FunctionAST(std::unique_ptr<PrototypeAST> Proto,
              std::unique_ptr<ExprAST> Body)
      : Proto(std::move(Proto)), Body(std::move(Body)) {}
};

#endif