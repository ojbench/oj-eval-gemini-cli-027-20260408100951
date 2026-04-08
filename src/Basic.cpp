#include <iostream>
#include <memory>
#include <string>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "Program.hpp"
#include "Token.hpp"
#include "utils/Error.hpp"

int main() {
  Lexer lexer;
  Parser parser;
  Program program;

  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      continue;
    }
    try {
      TokenStream tokens = lexer.tokenize(line);
      if (tokens.empty()) {
        continue;
      }

      const Token* firstToken = tokens.peek();
      if (firstToken->type == TokenType::RUN) {
        program.run();
      } else if (firstToken->type == TokenType::LIST) {
        program.list();
      } else if (firstToken->type == TokenType::CLEAR) {
        program.clear();
      } else if (firstToken->type == TokenType::QUIT) {
        break;
      } else if (firstToken->type == TokenType::HELP) {
        std::cout << "HELP\n"; // TODO: actual help message if needed
      } else {
        ParsedLine parsedLine = parser.parseLine(tokens, line);
        if (parsedLine.getLine().has_value()) {
          int lineNumber = parsedLine.getLine().value();
          auto stmt = parsedLine.fetchStatement();
          if (stmt) {
            program.addStmt(lineNumber, stmt);
          } else {
            program.removeStmt(lineNumber);
          }
        } else {
          auto stmt = parsedLine.fetchStatement();
          if (stmt) {
            program.execute(stmt);
          }
        }
      }
    } catch (const BasicError& e) {
      std::cout << e.message() << "\n";
    }
  }
  return 0;
}