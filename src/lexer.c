#include "lexer.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct {
  char *curr;
  int line;
} lexer;

lexer glexer;

void init_lexer(char *source) {
  glexer.curr = source;
  glexer.line = 1;
}

static char advance() { return *glexer.curr++; }

static char peek() { return *(glexer.curr + 1); }

static void skip_whitespace() {
  for (;;) {
    switch (advance()) {
    case '\r':
      if (peek() == '\n') {
        advance();
      }
    case '\n':
      glexer.line++;
      break;
    case '\t':
    case ' ':
      break;
    default:
      return;
    }
  }
}

static token *create_token(token_type type, char *lexeme) {
  token *new_token = malloc(sizeof(token));
  new_token->line = glexer.line;
  new_token->type = type;
  new_token->lexeme = lexeme;
  return new_token;
}

token *get_token() {
  for (;;) {
    skip_whitespace();
    switch (advance()) {
    case '(':
      return create_token(TOKEN_LEFT_PAREN, NULL);
    case ')':
      return create_token(TOKEN_RIGHT_PAREN, NULL);
    case '{':
      return create_token(TOKEN_LEFT_BRACE, NULL);
    case '/': {
      if (peek() == '=') {
        advance();
        return create_token(TOKEN_SLASH_EQUAL, NULL);
      } else {
        return create_token(TOKEN_SLASH, NULL);
      }
    }
    case '}':
      return create_token(TOKEN_RIGHT_BRACE, NULL);
    case '[':
      return create_token(TOKEN_LEFT_BRACKET, NULL);
    case ']':
      return create_token(TOKEN_RIGHT_BRACKET, NULL);
    case '"':
      break;
    case '=': {
      if (peek() == '=') {
        advance();
        return create_token(TOKEN_EQUAL_EQUAL, NULL);
      } else {
        return create_token(TOKEN_EQUAL, NULL);
      }
    }
    case '!':
      if (peek() == '=') {
        advance();
        return create_token(TOKEN_BANG_EQUAL, NULL);
      } else {
        return create_token(TOKEN_BANG, NULL);
      }
    case '+': {
      switch (peek()) {
      case '=':
        return create_token(TOKEN_PLUS_EQUAL, NULL);
      case '+':
        return create_token(TOKEN_PLUS_PLUS, NULL);
      default:
        return create_token(TOKEN_PLUS, NULL);
      }
    }
    case '-': {
      switch (peek()) {
      case '=':
        return create_token(TOKEN_MINUS_EQUAL, NULL);
      case '-':
        return create_token(TOKEN_MINUS_MINUS, NULL);
      default:
        return create_token(TOKEN_MINUS, NULL);
      }
    }
    case '*': {
      if (peek() == '=') {
        return create_token(TOKEN_STAR_EQUAL, NULL);
      } else {
        return create_token(TOKEN_STAR, NULL);
      }
    }
    case '<': {
      if (peek() == '=') {
        return create_token(TOKEN_LESS_EQUAL, NULL);
      } else {
        return create_token(TOKEN_LESS, NULL);
      }
    }
    case '>': {
      if (peek() == '=') {
        return create_token(TOKEN_GREATER_EQUAL, NULL);
      } else {
        return create_token(TOKEN_GREATER, NULL);
      }
    }
    case '|': {
      if (peek() == '|') {
        return create_token(TOKEN_OR, NULL);
      } else {
        return create_token(TOKEN_ERROR, NULL);
      }
    }
    case '&': {
      if (peek() == '&') {
        return create_token(TOKEN_OR, NULL);
      } else {
        return create_token(TOKEN_ERROR, NULL);
      }
    }
    case '%':
      return create_token(TOKEN_PERCENT, NULL);
    case '?':
      return create_token(TOKEN_QUESTION, NULL);
    case ':':
      return create_token(TOKEN_COLON, NULL);
    case '\0':
      return create_token(TOKEN_EOF, NULL);
    default: {
      break;
    }
    }
  }
}

char *type_to_string(token_type type) {
  switch (type) {
  case TOKEN_AND:
    return "&&";
  case TOKEN_BANG:
    return "!";
  case TOKEN_BANG_EQUAL:
    return "!=";
  case TOKEN_BOOL:
    return "bool";
  case TOKEN_COLON:
    return ":";
  case TOKEN_EQUAL:
    return "=";
  case TOKEN_EQUAL_EQUAL:
    return "==";
  case TOKEN_EOF:
    return "EOF";
  case TOKEN_ERROR:
    return "error";
  case TOKEN_FLOAT:
    return "float";
  case TOKEN_GREATER:
    return ">";
  case TOKEN_GREATER_EQUAL:
    return ">=";
  case TOKEN_IDENTIFIER:
    return "identifier";
  case TOKEN_INT:
    return "int";
  case TOKEN_LEFT_BRACE:
    return "{";
  case TOKEN_LEFT_BRACKET:
    return "]";
  case TOKEN_LEFT_PAREN:
    return "(";
  case TOKEN_LESS:
    return "<";
  case TOKEN_LESS_EQUAL:
    return "<=";
  case TOKEN_MINUS:
    return "-";
  case TOKEN_MINUS_EQUAL:
    return "-=";
  case TOKEN_MINUS_MINUS:
    return "--";
  case TOKEN_OR:
    return "||";
  case TOKEN_PERCENT:
    return "%";
  case TOKEN_PLUS:
    return "+";
  case TOKEN_PLUS_EQUAL:
    return "+=";
  case TOKEN_PLUS_PLUS:
    return "++";
  case TOKEN_QUESTION:
    return "?";
  case TOKEN_RIGHT_BRACE:
    return "}";
  case TOKEN_RIGHT_BRACKET:
    return "]";
  case TOKEN_RIGHT_PAREN:
    return ")";
  case TOKEN_SLASH:
    return "/";
  case TOKEN_SLASH_EQUAL:
    return "/=";
  case TOKEN_STAR:
    return "*";
  case TOKEN_STAR_EQUAL:
    return "*=";
  case TOKEN_STRING:
    return "str";
  }
  return "invalid token";
}
