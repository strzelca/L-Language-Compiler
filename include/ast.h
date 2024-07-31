#ifndef AST_H
#define AST_H

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>
class ExprAST {
public:
  virtual ~ExprAST() = default;
  virtual llvm::Value *codegen() = 0;
};

class NumberExprAST : public ExprAST {
  double val;

public:
  NumberExprAST(double val) : val(val) {}
  llvm::Value *codegen() override;
};

class VariableExprAST : public ExprAST {
  std::string Id;

public:
  VariableExprAST(const std::string &Id) : Id(Id) {}
  llvm::Value *codegen() override;
};

class BinaryExprAST : public ExprAST {
  char Op;
  std::unique_ptr<ExprAST> LHS, RHS;

public:
  BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
                std::unique_ptr<ExprAST> RHS)
      : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
  llvm::Value *codegen() override;
};

class UnaryExprAST : public ExprAST {
  char Op;
  std::unique_ptr<ExprAST> HS;

public:
  UnaryExprAST(char Op, std::unique_ptr<ExprAST> HS)
      : Op(Op), HS(std::move(HS)) {}
  llvm::Value *codegen() override;
};

class CallExprAST : public ExprAST {
  std::string Callee;
  std::vector<std::unique_ptr<ExprAST>> Args;

public:
  CallExprAST(const std::string &Callee,
              std::vector<std::unique_ptr<ExprAST>> Args)
      : Callee(Callee), Args(std::move(Args)) {}
  llvm::Value *codegen() override;
};

class PrototypeAST {
  std::string Id;
  std::vector<std::string> Args;

public:
  PrototypeAST(const std::string &Id, std::vector<std::string> Args)
      : Id(Id), Args(std::move(Args)) {}
  llvm::Function *codegen();
  const std::string &getId() const { return Id; }
};

class FunctionAST {
  std::string Id;
  std::unique_ptr<PrototypeAST> Proto;
  std::unique_ptr<ExprAST> Body;

public:
  FunctionAST(std::unique_ptr<PrototypeAST> Proto,
              std::unique_ptr<ExprAST> Body)
      : Id("main"), Proto(std::move(Proto)), Body(std::move(Body)) {}
  llvm::Function *codegen();
  const std::string &getId() const {
    if (!Proto)
      return Id;
    return Proto->getId();
  }
};

#endif