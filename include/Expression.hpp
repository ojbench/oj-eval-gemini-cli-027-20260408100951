#pragma once

#include <memory>
#include <string>

class VarState;

class Expression {
 public:
  virtual ~Expression() = default;
  virtual int evaluate(const VarState& state) const = 0;
};

class ConstExpression : public Expression {
 public:
  explicit ConstExpression(int value);
  ~ConstExpression() = default;
  int evaluate(const VarState& state) const override;

 private:
  int value_;
};

class VariableExpression : public Expression {
 public:
  explicit VariableExpression(std::string name);
  ~VariableExpression() = default;
  int evaluate(const VarState& state) const override;

 private:
  std::string name_;
};

class CompoundExpression : public Expression {
 public:
  CompoundExpression(std::shared_ptr<Expression> left, char op, std::shared_ptr<Expression> right);
  ~CompoundExpression() = default;
  int evaluate(const VarState& state) const override;

 private:
  std::shared_ptr<Expression> left_;
  std::shared_ptr<Expression> right_;
  char op_;
};
