#include "Statement.hpp"

#include <iostream>
#include <limits>
#include <sstream>
#include <utility>

#include "Program.hpp"
#include "VarState.hpp"
#include "utils/Error.hpp"

Statement::Statement(std::string source) : source_(std::move(source)) {}

const std::string& Statement::text() const noexcept { return source_; }

LetStatement::LetStatement(std::string source, std::string varName, std::shared_ptr<Expression> expr)
    : Statement(std::move(source)), varName_(std::move(varName)), expr_(std::move(expr)) {}

void LetStatement::execute(VarState& state, Program& program) const {
  state.setValue(varName_, expr_->evaluate(state));
}

PrintStatement::PrintStatement(std::string source, std::shared_ptr<Expression> expr)
    : Statement(std::move(source)), expr_(std::move(expr)) {}

void PrintStatement::execute(VarState& state, Program& program) const {
  std::cout << expr_->evaluate(state) << "\n";
}

InputStatement::InputStatement(std::string source, std::string varName)
    : Statement(std::move(source)), varName_(std::move(varName)) {}

void InputStatement::execute(VarState& state, Program& program) const {
  int value;
  std::string input;
  while (true) {
    std::cout << " ? ";
    if (!std::getline(std::cin, input)) {
      throw BasicError("EOF");
    }
    try {
      size_t pos;
      value = std::stoi(input, &pos);
      while (pos < input.length() && std::isspace(input[pos])) {
        pos++;
      }
      if (pos != input.length()) {
        throw std::invalid_argument("");
      }
      break;
    } catch (...) {
      std::cout << "INVALID NUMBER\n";
    }
  }
  state.setValue(varName_, value);
}

GotoStatement::GotoStatement(std::string source, int targetLine)
    : Statement(std::move(source)), targetLine_(targetLine) {}

void GotoStatement::execute(VarState& state, Program& program) const {
  program.changePC(targetLine_);
}

IfStatement::IfStatement(std::string source, std::shared_ptr<Expression> left, char op, std::shared_ptr<Expression> right, int targetLine)
    : Statement(std::move(source)), left_(std::move(left)), right_(std::move(right)), op_(op), targetLine_(targetLine) {}

void IfStatement::execute(VarState& state, Program& program) const {
  int lhs = left_->evaluate(state);
  int rhs = right_->evaluate(state);
  bool condition = false;
  switch (op_) {
    case '=': condition = (lhs == rhs); break;
    case '<': condition = (lhs < rhs); break;
    case '>': condition = (lhs > rhs); break;
  }
  if (condition) {
    program.changePC(targetLine_);
  }
}

RemStatement::RemStatement(std::string source) : Statement(std::move(source)) {}

void RemStatement::execute(VarState& state, Program& program) const {}

EndStatement::EndStatement(std::string source) : Statement(std::move(source)) {}

void EndStatement::execute(VarState& state, Program& program) const {
  program.programEnd();
}

IndentStatement::IndentStatement(std::string source) : Statement(std::move(source)) {}

void IndentStatement::execute(VarState& state, Program& program) const {
  state.indent();
}

DedentStatement::DedentStatement(std::string source) : Statement(std::move(source)) {}

void DedentStatement::execute(VarState& state, Program& program) const {
  state.dedent();
}
