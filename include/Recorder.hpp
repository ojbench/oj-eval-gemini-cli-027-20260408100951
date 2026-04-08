#pragma once

#include <map>
#include <memory>
#include <vector>

#include "Statement.hpp"

class Recorder {
 public:
  ~Recorder() = default;

  void add(int line, std::shared_ptr<Statement> stmt);
  void remove(int line);
  std::shared_ptr<Statement> get(int line) const noexcept;
  bool hasLine(int line) const noexcept;
  void clear() noexcept;
  void printLines() const;
  int nextLine(int line) const noexcept;
  int firstLine() const noexcept;

 private:
  std::map<int, std::shared_ptr<Statement>> lines_;
};
