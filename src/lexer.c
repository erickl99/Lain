#include "lexer.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *curr;
  int line;
} lexer;

lexer glexer;

void init_lexer(char *source) {
  glexer.curr = source;
  glexer.line = 1;
}

static token *create_token(token_type type) {
  token *new_token = malloc(sizeof(token));
  new_token->line = glexer.line;
  new_token->type = type;
  new_token->lexeme = NULL;
  return new_token;
}

static token *create_full_token(token_type type, char *lexeme) {
  token *new_token = malloc(sizeof(token));
  new_token->line = glexer.line;
  new_token->type = type;
  new_token->lexeme = lexeme;
  return new_token;
}

static token *error_token(char *message) {
  token *new_token = malloc(sizeof(token));
  new_token->line = glexer.line;
  new_token->type = TOKEN_ERROR;
  new_token->lexeme = message;
  return new_token;
}

static char advance() { return *glexer.curr++; }

static char peek() { return *glexer.curr; }

static bool finished() { return *glexer.curr == '\0'; }

static bool is_alpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}

static bool is_digit(char c) { return c >= '0' && c <= '9'; }

static token *identifier() { return create_token(TOKEN_IDENTIFIER); }

//TODO: properly handle any whitespace or line breaks, support numbers that start with just a dot
static token *number(int sign) { 
  bool is_float = false;
  char *start = glexer.curr - 1;
  while (is_digit(peek())) {
    advance();
  }
  if (peek() == '.') {
    advance();
    is_float = true;
  }
  while (is_digit(peek())) {
    advance();
  }
  char *end = glexer.curr;
  size_t size = end - start;
  char *num_str = malloc(size + 1);
  memcpy(num_str, start, size);
  num_str[size] = '\0';
  printf("This is what we put into num_str: |%s|\n", num_str);
  free(num_str);
  advance();
  return create_token(TOKEN_INT);
}

//TODO: add proper support for multiline strings
static token *string() {
  char *start = glexer.curr;
  while (peek() != '"' && !finished()) {
    advance();
  }
  if (finished()) {
    return error_token("unexpected terminated string literal");
  }
  char *end = glexer.curr;
  size_t size = end - start;
  char *str = malloc(size + 1);
  memcpy(str, start, size);
  str[size] = '\0';
  advance();
  printf("Created string %s\n", str);
  return create_full_token(TOKEN_STRING, str);
}

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
      glexer.curr--;
      return;
    }
  }
}

token *get_token() {
  skip_whitespace();
  char c = advance();
  if (is_alpha(c)) {
    return identifier();
  } else if (is_digit(c)) {
    return number(1);
  }
  switch (c) {
  case '"':
    return string();
  case '(':
    return create_token(TOKEN_LEFT_PAREN);
  case ')':
    return create_token(TOKEN_RIGHT_PAREN);
  case '{':
    return create_token(TOKEN_LEFT_BRACE);
  case '}':
    return create_token(TOKEN_RIGHT_BRACE);
  case '[':
    return create_token(TOKEN_LEFT_BRACKET);
  case ']':
    return create_token(TOKEN_RIGHT_BRACKET);
  case '+': {
    switch (peek()) {
    case '=':
      advance();
      return create_token(TOKEN_PLUS_EQUAL);
    case '+':
      advance();
      return create_token(TOKEN_PLUS_PLUS);
    default:
      return create_token(TOKEN_PLUS);
    }
  }
  case '-': {
    char d = peek();
    switch (peek()) {
    case '=':
      advance();
      return create_token(TOKEN_MINUS_EQUAL);
    case '-':
      advance();
      return create_token(TOKEN_MINUS_MINUS);
    default:
      if (is_digit(d)) {
        return number(-1);
      }
      return create_token(TOKEN_MINUS);
    }
  }
  case '*': {
    if (peek() == '=') {
      advance();
      return create_token(TOKEN_STAR_EQUAL);
    } else {
      return create_token(TOKEN_STAR);
    }
  }
  case '/': {
    if (peek() == '=') {
      advance();
      return create_token(TOKEN_SLASH_EQUAL);
    } else {
      return create_token(TOKEN_SLASH);
    }
  }
  case '=': {
    if (peek() == '=') {
      advance();
      return create_token(TOKEN_EQUAL_EQUAL);
    } else {
      return create_token(TOKEN_EQUAL);
    }
  }
  case '<': {
    if (peek() == '=') {
      advance();
      return create_token(TOKEN_LESS_EQUAL);
    } else {
      return create_token(TOKEN_LESS);
    }
  }
  case '>': {
    if (peek() == '=') {
      advance();
      return create_token(TOKEN_GREATER_EQUAL);
    } else {
      return create_token(TOKEN_GREATER);
    }
  }
  case '!': {
    if (peek() == '=') {
      advance();
      return create_token(TOKEN_BANG_EQUAL);
    } else {
      return create_token(TOKEN_BANG);
    }
  }
  case '&': {
    if (peek() == '&') {
      advance();
      return create_token(TOKEN_AND);
    } else {
      return error_token("invalid syntax");
    }
  }
  case '|': {
    if (peek() == '|') {
      advance();
      return create_token(TOKEN_OR);
    } else {
      return error_token("invalid syntax");
    }
  }
  case '%':
    return create_token(TOKEN_PERCENT);
  case '?':
    return create_token(TOKEN_QUESTION);
  case ':':
    return create_token(TOKEN_COLON);
  case '\0':
    return create_token(TOKEN_EOF);
  default:
    return error_token("Encountered unexpected character");
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
    return "[";
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
