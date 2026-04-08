#include "Program.hpp"
#include "Statement.hpp"
#include "utils/Error.hpp"

Program::Program() : programCounter_(-1), programEnd_(false) {}

void Program::addStmt(int line, std::shared_ptr<Statement> stmt) {
  recorder_.add(line, stmt);
}

void Program::removeStmt(int line) {
  recorder_.remove(line);
}

void Program::run() {
  programCounter_ = recorder_.firstLine();
  programEnd_ = false;

  while (programCounter_ != -1 && !programEnd_) {
    auto stmt = recorder_.get(programCounter_);
    if (!stmt) {
      throw BasicError("LINE NUMBER ERROR");
    }
    int currentLine = programCounter_;
    stmt->execute(vars_, *this);
    if (programCounter_ == currentLine) {
      programCounter_ = recorder_.nextLine(programCounter_);
    }
  }
  resetAfterRun();
}

void Program::list() const {
  recorder_.printLines();
}

void Program::clear() {
  recorder_.clear();
  vars_.clear();
  resetAfterRun();
}

void Program::execute(std::shared_ptr<Statement> stmt) {
  stmt->execute(vars_, *this);
}

int Program::getPC() const noexcept {
  return programCounter_;
}

void Program::changePC(int line) {
  if (!recorder_.hasLine(line)) {
    throw BasicError("LINE NUMBER ERROR");
  }
  programCounter_ = line;
}

void Program::programEnd() {
  programEnd_ = true;
}

void Program::resetAfterRun() noexcept {
  programCounter_ = -1;
  programEnd_ = false;
}