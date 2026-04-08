#include "Parser.hpp"

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Expression.hpp"
#include "Statement.hpp"
#include "utils/Error.hpp"

ParsedLine::ParsedLine() { statement_ = nullptr; }

void ParsedLine::setLine(int line) { line_number_.emplace(line); }

std::optional<int> ParsedLine::getLine() { return line_number_; }

void ParsedLine::setStatement(std::shared_ptr<Statement> stmt) { statement_ = stmt; }

std::shared_ptr<Statement> ParsedLine::getStatement() const { return statement_; }

std::shared_ptr<Statement> ParsedLine::fetchStatement() {
  auto temp = statement_;
  statement_ = nullptr;
  return temp;
}

ParsedLine Parser::parseLine(TokenStream& tokens,
                             const std::string& originLine) const {
  ParsedLine result;

  const Token* firstToken = tokens.peek();
  if (firstToken && firstToken->type == TokenType::NUMBER) {
    result.setLine(parseLiteral(firstToken));
    tokens.get();

    if (tokens.empty()) {
      return result;
    }
  }

  result.setStatement(parseStatement(tokens, originLine));

  return result;
}

std::shared_ptr<Statement> Parser::parseStatement(TokenStream& tokens,
                                  const std::string& originLine) const {
  if (tokens.empty()) {
    throw BasicError("SYNTAX ERROR");
  }

  const Token* token = tokens.get();
  if (!token) {
    throw BasicError("SYNTAX ERROR");
  }

  switch (token->type) {
    case TokenType::LET:
      return parseLet(tokens, originLine);
    case TokenType::PRINT:
      return parsePrint(tokens, originLine);
    case TokenType::INPUT:
      return parseInput(tokens, originLine);
    case TokenType::GOTO:
      return parseGoto(tokens, originLine);
    case TokenType::IF:
      return parseIf(tokens, originLine);
    case TokenType::REM:
      return parseRem(tokens, originLine);
    case TokenType::END:
      return parseEnd(tokens, originLine);
    case TokenType::INDENT:
      return parseIndent(tokens, originLine);
    case TokenType::DEDENT:
      return parseDedent(tokens, originLine);
    default:
      throw BasicError("SYNTAX ERROR");
  }
}

std::shared_ptr<Statement> Parser::parseLet(TokenStream& tokens,
                            const std::string& originLine) const {
  if (tokens.empty()) {
    throw BasicError("SYNTAX ERROR");
  }

  const Token* varToken = tokens.get();
  if (!varToken || varToken->type != TokenType::IDENTIFIER) {
    throw BasicError("SYNTAX ERROR");
  }

  std::string varName = varToken->text;

  if (tokens.empty() || tokens.get()->type != TokenType::EQUAL) {
    throw BasicError("SYNTAX ERROR");
  }

  auto expr = parseExpression(tokens);

  return std::make_shared<LetStatement>(originLine, varName, expr);
}

std::shared_ptr<Statement> Parser::parsePrint(TokenStream& tokens,
                              const std::string& originLine) const {
  auto expr = parseExpression(tokens);
  return std::make_shared<PrintStatement>(originLine, expr);
}

std::shared_ptr<Statement> Parser::parseInput(TokenStream& tokens,
                              const std::string& originLine) const {
  if (tokens.empty()) {
    throw BasicError("SYNTAX ERROR");
  }

  const Token* varToken = tokens.get();
  if (!varToken || varToken->type != TokenType::IDENTIFIER) {
    throw BasicError("SYNTAX ERROR");
  }

  std::string varName = varToken->text;
  return std::make_shared<InputStatement>(originLine, varName);
}

std::shared_ptr<Statement> Parser::parseGoto(TokenStream& tokens,
                             const std::string& originLine) const {
  if (tokens.empty()) {
    throw BasicError("SYNTAX ERROR");
  }

  const Token* lineToken = tokens.get();
  if (!lineToken || lineToken->type != TokenType::NUMBER) {
    throw BasicError("SYNTAX ERROR");
  }

  int targetLine = parseLiteral(lineToken);
  return std::make_shared<GotoStatement>(originLine, targetLine);
}

std::shared_ptr<Statement> Parser::parseIf(TokenStream& tokens,
                           const std::string& originLine) const {
  auto leftExpr = parseExpression(tokens);

  if (tokens.empty()) {
    throw BasicError("SYNTAX ERROR");
  }

  const Token* opToken = tokens.get();
  char op;
  switch (opToken->type) {
    case TokenType::EQUAL:
      op = '=';
      break;
    case TokenType::GREATER:
      op = '>';
      break;
    case TokenType::LESS:
      op = '<';
      break;
    default:
      throw BasicError("SYNTAX ERROR");
  }

  auto rightExpr = parseExpression(tokens);

  if (tokens.empty() || tokens.get()->type != TokenType::THEN) {
    throw BasicError("SYNTAX ERROR");
  }

  if (tokens.empty()) {
    throw BasicError("SYNTAX ERROR");
  }

  const Token* lineToken = tokens.get();
  if (!lineToken || lineToken->type != TokenType::NUMBER) {
    throw BasicError("SYNTAX ERROR");
  }

  int targetLine = parseLiteral(lineToken);

  return std::make_shared<IfStatement>(originLine, leftExpr, op, rightExpr, targetLine);
}

std::shared_ptr<Statement> Parser::parseRem(TokenStream& tokens,
                            const std::string& originLine) const {
  const Token* remInfo = tokens.get();
  if (!remInfo || remInfo->type != TokenType::REMINFO) {
    throw BasicError("SYNTAX ERROR");
  }
  return std::make_shared<RemStatement>(originLine);
}

std::shared_ptr<Statement> Parser::parseEnd(TokenStream& tokens,
                            const std::string& originLine) const {
  return std::make_shared<EndStatement>(originLine);
}

std::shared_ptr<Statement> Parser::parseIndent(TokenStream& tokens,
                            const std::string& originLine) const {
  return std::make_shared<IndentStatement>(originLine);
}

std::shared_ptr<Statement> Parser::parseDedent(TokenStream& tokens,
                            const std::string& originLine) const {
  return std::make_shared<DedentStatement>(originLine);
}

std::shared_ptr<Expression> Parser::parseExpression(TokenStream& tokens) const {
  return parseExpression(tokens, 0);
}

std::shared_ptr<Expression> Parser::parseExpression(TokenStream& tokens, int precedence) const {
  std::shared_ptr<Expression> left;

  if (tokens.empty()) {
    throw BasicError("SYNTAX ERROR");
  }

  const Token* token = tokens.get();
  if (!token) {
    throw BasicError("SYNTAX ERROR");
  }

  if (token->type == TokenType::NUMBER) {
    int value = parseLiteral(token);
    left = std::make_shared<ConstExpression>(value);
  } else if (token->type == TokenType::IDENTIFIER) {
    left = std::make_shared<VariableExpression>(token->text);
  } else if (token->type == TokenType::LEFT_PAREN) {
    ++leftParentCount;
    left = parseExpression(tokens, 0);

    if (tokens.empty() || tokens.get()->type != TokenType::RIGHT_PAREN) {
      throw BasicError("MISMATCHED PARENTHESIS");
    }
    --leftParentCount;
  } else {
    throw BasicError("SYNTAX ERROR");
  }

  while (!tokens.empty()) {
    const Token* opToken = tokens.peek();
    if (!opToken) {
      break;
    }

    if (opToken->type == TokenType::RIGHT_PAREN) {
      if (leftParentCount == 0) {
        throw BasicError("MISMATCHED PARENTHESIS");
      }
      break;
    }

    int opPrecedence = getPrecedence(opToken->type);
    if (opPrecedence == -1 || opPrecedence < precedence) {
      break;
    }

    tokens.get();

    char op;
    switch (opToken->type) {
      case TokenType::PLUS:
        op = '+';
        break;
      case TokenType::MINUS:
        op = '-';
        break;
      case TokenType::MUL:
        op = '*';
        break;
      case TokenType::DIV:
        op = '/';
        break;
      default:
        throw BasicError("SYNTAX ERROR");
    }

    auto right = parseExpression(tokens, opPrecedence + 1);
    left = std::make_shared<CompoundExpression>(left, op, right);
  }

  return left;
}

int Parser::getPrecedence(TokenType op) const {
  switch (op) {
    case TokenType::PLUS:
    case TokenType::MINUS:
      return 1;
    case TokenType::MUL:
    case TokenType::DIV:
      return 2;
    default:
      return -1;
  }
}

int Parser::parseLiteral(const Token* token) const {
  if (!token || token->type != TokenType::NUMBER) {
    throw BasicError("SYNTAX ERROR");
  }

  try {
    size_t pos;
    int value = std::stoi(token->text, &pos);

    if (pos != token->text.length()) {
      throw BasicError("INT LITERAL OVERFLOW");
    }

    return value;
  } catch (const std::out_of_range&) {
    throw BasicError("INT LITERAL OVERFLOW");
  } catch (const std::invalid_argument&) {
    throw BasicError("SYNTAX ERROR");
  }
}