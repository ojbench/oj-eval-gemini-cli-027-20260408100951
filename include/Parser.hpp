#pragma once

#include <memory>
#include <optional>

#include "Token.hpp"

class Statement;
class Expression;

class ParsedLine {
 private:
  std::optional<int> line_number_;
  std::shared_ptr<Statement> statement_;

 public:
  ParsedLine();
  ~ParsedLine() = default;

  void setLine(int line);
  std::optional<int> getLine();
  void setStatement(std::shared_ptr<Statement> stmt);
  std::shared_ptr<Statement> getStatement() const;
  std::shared_ptr<Statement> fetchStatement();
};

class Parser {
 public:
  ParsedLine parseLine(TokenStream& tokens,
                       const std::string& originLine) const;

 private:
  std::shared_ptr<Statement> parseStatement(TokenStream& tokens,
                            const std::string& originLine) const;
  std::shared_ptr<Statement> parseLet(TokenStream& tokens, const std::string& originLine) const;
  std::shared_ptr<Statement> parsePrint(TokenStream& tokens,
                        const std::string& originLine) const;
  std::shared_ptr<Statement> parseInput(TokenStream& tokens,
                        const std::string& originLine) const;
  std::shared_ptr<Statement> parseGoto(TokenStream& tokens,
                       const std::string& originLine) const;
  std::shared_ptr<Statement> parseIf(TokenStream& tokens, const std::string& originLine) const;
  std::shared_ptr<Statement> parseRem(TokenStream& tokens, const std::string& originLine) const;
  std::shared_ptr<Statement> parseEnd(TokenStream& tokens, const std::string& originLine) const;
  std::shared_ptr<Statement> parseIndent(TokenStream& tokens, const std::string& originLine) const;
  std::shared_ptr<Statement> parseDedent(TokenStream& tokens, const std::string& originLine) const;

  std::shared_ptr<Expression> parseExpression(TokenStream& tokens) const;
  std::shared_ptr<Expression> parseExpression(TokenStream& tokens, int precedence) const;

  int getPrecedence(TokenType op) const;
  int parseLiteral(const Token* token) const;

  mutable int leftParentCount{0};
};
