#pragma once

#include <memory>
#include <string>

#include "Expression.hpp"

class Program;
class VarState;

class Statement {
 public:
  explicit Statement(std::string source);
  virtual ~Statement() = default;

  virtual void execute(VarState& state, Program& program) const = 0;

  const std::string& text() const noexcept;

 private:
  std::string source_;
};

class LetStatement : public Statement {
 public:
  LetStatement(std::string source, std::string varName, std::shared_ptr<Expression> expr);
  void execute(VarState& state, Program& program) const override;

 private:
  std::string varName_;
  std::shared_ptr<Expression> expr_;
};

class PrintStatement : public Statement {
 public:
  PrintStatement(std::string source, std::shared_ptr<Expression> expr);
  void execute(VarState& state, Program& program) const override;

 private:
  std::shared_ptr<Expression> expr_;
};

class InputStatement : public Statement {
 public:
  InputStatement(std::string source, std::string varName);
  void execute(VarState& state, Program& program) const override;

 private:
  std::string varName_;
};

class GotoStatement : public Statement {
 public:
  GotoStatement(std::string source, int targetLine);
  void execute(VarState& state, Program& program) const override;

 private:
  int targetLine_;
};

class IfStatement : public Statement {
 public:
  IfStatement(std::string source, std::shared_ptr<Expression> left, char op, std::shared_ptr<Expression> right, int targetLine);
  void execute(VarState& state, Program& program) const override;

 private:
  std::shared_ptr<Expression> left_;
  std::shared_ptr<Expression> right_;
  char op_;
  int targetLine_;
};

class RemStatement : public Statement {
 public:
  RemStatement(std::string source);
  void execute(VarState& state, Program& program) const override;
};

class EndStatement : public Statement {
 public:
  EndStatement(std::string source);
  void execute(VarState& state, Program& program) const override;
};

class IndentStatement : public Statement {
 public:
  IndentStatement(std::string source);
  void execute(VarState& state, Program& program) const override;
};

class DedentStatement : public Statement {
 public:
  DedentStatement(std::string source);
  void execute(VarState& state, Program& program) const override;
};
