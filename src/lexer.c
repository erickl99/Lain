#include "lexer.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *curr;
  int line;
  token *tkn;
} lexer;

lexer glexer;

void init_lexer(char *source, token *tkn) {
  glexer.curr = source;
  glexer.line = 1;
  glexer.tkn = tkn;
}

static token *create_basic_token(token_type type) {
  glexer.tkn->line = glexer.line;
  glexer.tkn->type = type;
  glexer.tkn->size = 0;
  glexer.tkn->lexeme = NULL;
  return glexer.tkn;
}

static token *create_full_token(token_type type, char *lexeme, size_t size) {
  glexer.tkn->line = glexer.line;
  glexer.tkn->type = type;
  glexer.tkn->size = size;
  glexer.tkn->lexeme = lexeme;
  return glexer.tkn;
}

static token *error_token(char *message) {
  glexer.tkn->line = glexer.line;
  glexer.tkn->type = TOKEN_ERROR;
  glexer.tkn->lexeme = message;
  glexer.tkn->size = strlen(message);
  return glexer.tkn;
}

static char advance() { return *glexer.curr++; }

static char peek() { return *glexer.curr; }

static bool finished() { return *glexer.curr == '\0'; }

static bool is_alpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}

static bool is_digit(char c) { return c >= '0' && c <= '9'; }

static bool is_alphanum(char c) { return is_alpha(c) || is_digit(c); }

static bool newline() {
  char c = *glexer.curr;
  return c == '\r' || c == '\n';
}

static bool compare_str(char *start, size_t length, char *word, size_t size) {
  if (length != size) {
    return false;
  }
  return memcmp(start, word, length) == 0;
}

static token *keyword_identifier(char *start, char *end) {
  size_t length = end - start;
  switch (*start) {
  case 'c': {
    switch (*(start + 1)) {
    case 'a':
      if (compare_str(start + 2, length - 2, "se", 2)) {
        return create_basic_token(TOKEN_CASE);
      }
      break;
    case 'o':
      if (compare_str(start + 2, length - 2, "nst", 3)) {
        return create_basic_token(TOKEN_CONST);
      }
      break;
    }
    break;
  }
  case 'd':
    if (compare_str(start + 1, length - 1, "efault", 6)) {
      return create_basic_token(TOKEN_DEFAULT);
    }
    break;
  case 'e':
    if (compare_str(start + 1, length - 1, "lse", 3)) {
      return create_basic_token(TOKEN_ELSE);
    }
    break;
  case 'f': {
    switch (*(start + 1)) {
    case 'a':
      if (compare_str(start + 2, length - 2, "lse", 3)) {
        return create_basic_token(TOKEN_FALSE);
      }
      break;
    case 'o':
      if (compare_str(start + 2, length - 2, "r", 1)) {
        return create_basic_token(TOKEN_FOR);
      }
      break;
    }
    break;
  }
  case 'i':
    if (compare_str(start + 1, length - 1, "f", 1)) {
      return create_basic_token(TOKEN_IF);
    }
    break;
  case 'r':
    if (compare_str(start + 1, length - 1, "eturn", 5)) {
      return create_basic_token(TOKEN_RETURN);
    }
    break;
  case 's':
    if (compare_str(start + 1, length - 1, "witch", 5)) {
      return create_basic_token(TOKEN_SWITCH);
    }
    break;
  case 't': {
    switch (*(start + 1)) {
    case 'r':
      if (compare_str(start + 2, length - 2, "ue", 2)) {
        return create_basic_token(TOKEN_TRUE);
      }
      break;
    case 'y':
      if (compare_str(start + 2, length - 2, "pe", 2)) {
        return create_basic_token(TOKEN_TYPE);
      }
      break;
    }
    break;
  }
  case 'v':
    if (compare_str(start + 1, length - 1, "ar", 2)) {
      return create_basic_token(TOKEN_VAR);
    }
    break;
  }
  return create_full_token(TOKEN_IDENTIFIER, start, length);
}

static token *identifier() {
  char *start = glexer.curr - 1;
  while (is_alphanum(peek())) {
    advance();
  }
  char *end = glexer.curr;
  return keyword_identifier(start, end);
}

static token *number() {
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
  advance();
  if (is_float) {
    return create_full_token(TOKEN_FLOAT, start, size);
  } else {
    return create_full_token(TOKEN_INT, start, size);
  }
}

static token *string() {
  char *start = glexer.curr;
  while (peek() != '"' && !newline() && !finished()) {
    advance();
  }
  if (newline()) {
    return error_token("unexpected terminated string literal");
  }
  if (finished()) {
    return error_token("unexpected terminated string literal");
  }
  char *end = glexer.curr;
  size_t size = end - start;
  advance();
  return create_full_token(TOKEN_STRING, start, size);
}

static void skip_whitespace() {
  for (;;) {
    switch (advance()) {
    case '/': {
      if (peek() == '/') {
        advance();
        while (!newline() && !finished()) {
          advance();
        }
        if (!finished()) {
          glexer.line++;
          advance();
          continue;
        }
      }
      return;
    }
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
    return number();
  }
  switch (c) {
  case '"':
    return string();
  case '(':
    return create_basic_token(TOKEN_LEFT_PAREN);
  case ')':
    return create_basic_token(TOKEN_RIGHT_PAREN);
  case '{':
    return create_basic_token(TOKEN_LEFT_BRACE);
  case '}':
    return create_basic_token(TOKEN_RIGHT_BRACE);
  case '[':
    return create_basic_token(TOKEN_LEFT_BRACKET);
  case ']':
    return create_basic_token(TOKEN_RIGHT_BRACKET);
  case '+': {
    switch (peek()) {
    case '=':
      advance();
      return create_basic_token(TOKEN_PLUS_EQUAL);
    case '+':
      advance();
      return create_basic_token(TOKEN_PLUS_PLUS);
    default:
      return create_basic_token(TOKEN_PLUS);
    }
  }
  case '-': {
    char d = peek();
    switch (peek()) {
    case '=':
      advance();
      return create_basic_token(TOKEN_MINUS_EQUAL);
    case '-':
      advance();
      return create_basic_token(TOKEN_MINUS_MINUS);
    default:
      if (is_digit(d) || d == '.') {
        return number();
      }
      return create_basic_token(TOKEN_MINUS);
    }
  }
  case '*': {
    if (peek() == '=') {
      advance();
      return create_basic_token(TOKEN_STAR_EQUAL);
    } else {
      return create_basic_token(TOKEN_STAR);
    }
  }
  case '/': {
    if (peek() == '=') {
      advance();
      return create_basic_token(TOKEN_SLASH_EQUAL);
    } else {
      return create_basic_token(TOKEN_SLASH);
    }
  }
  case '=': {
    if (peek() == '=') {
      advance();
      return create_basic_token(TOKEN_EQUAL_EQUAL);
    } else {
      return create_basic_token(TOKEN_EQUAL);
    }
  }
  case '<': {
    if (peek() == '=') {
      advance();
      return create_basic_token(TOKEN_LESS_EQUAL);
    } else {
      return create_basic_token(TOKEN_LESS);
    }
  }
  case '>': {
    if (peek() == '=') {
      advance();
      return create_basic_token(TOKEN_GREATER_EQUAL);
    } else {
      return create_basic_token(TOKEN_GREATER);
    }
  }
  case '!': {
    if (peek() == '=') {
      advance();
      return create_basic_token(TOKEN_BANG_EQUAL);
    } else {
      return create_basic_token(TOKEN_BANG);
    }
  }
  case '&': {
    if (peek() == '&') {
      advance();
      return create_basic_token(TOKEN_AND);
    } else {
      return error_token("invalid syntax");
    }
  }
  case '|': {
    if (peek() == '|') {
      advance();
      return create_basic_token(TOKEN_OR);
    } else {
      return error_token("invalid syntax");
    }
  }
  case '%':
    return create_basic_token(TOKEN_PERCENT);
  case '.':
    if (is_digit(peek())) {
      return number();
    } else {
      return create_basic_token(TOKEN_DOT);
    }
  case '?':
    return create_basic_token(TOKEN_QUESTION);
  case ':':
    return create_basic_token(TOKEN_COLON);
  case '\0':
    return create_basic_token(TOKEN_EOF);
  default:
    return error_token("Encountered unexpected character");
  }
}

void token_to_string(token *tkn) {
  switch (tkn->type) {
  case TOKEN_AND:
    printf("AND && %d\n", tkn->line);
    return;
  case TOKEN_BANG:
    printf("BANG ! %d\n", tkn->line);
    return;
  case TOKEN_BANG_EQUAL:
    printf("BANG_EQUAL != %d\n", tkn->line);
    return;
  case TOKEN_CASE:
    printf("CASE case %d\n", tkn->line);
    return;
  case TOKEN_COLON:
    printf("COLON : %d\n", tkn->line);
    return;
  case TOKEN_CONST:
    printf("CONST const %d\n", tkn->line);
    return;
  case TOKEN_DEFAULT:
    printf("DEFAULT default %d\n", tkn->line);
    return;
  case TOKEN_DOT:
    printf("DOT . %d\n", tkn->line);
    return;
  case TOKEN_ELSE:
    printf("ELSE = %d\n", tkn->line);
    return;
  case TOKEN_EQUAL:
    printf("EQUAL = %d\n", tkn->line);
    return;
  case TOKEN_EQUAL_EQUAL:
    printf("EQUAL_EQUAL == %d\n", tkn->line);
    return;
  case TOKEN_EOF:
    printf("EOF %d\n", tkn->line);
    return;
  case TOKEN_ERROR:
    printf("ERROR: %s at line %d\n", tkn->lexeme, tkn->line);
    return;
  case TOKEN_FALSE:
    printf("FALSE false %d\n", tkn->line);
    return;
  case TOKEN_FLOAT:
    printf("FLOAT |%.*s| %d\n", (int)tkn->size, tkn->lexeme, tkn->line);
    return;
  case TOKEN_FOR:
    printf("FOR for %d\n", tkn->line);
    return;
  case TOKEN_GREATER:
    printf("GREATER > %d\n", tkn->line);
    return;
  case TOKEN_GREATER_EQUAL:
    printf("GREATER_EQUAL >= %d\n", tkn->line);
    return;
  case TOKEN_IDENTIFIER:
    printf("IDENTIFIER '%.*s' %d\n", (int)tkn->size, tkn->lexeme, tkn->line);
    return;
  case TOKEN_IF:
    printf("IF if %d\n", tkn->line);
    return;
  case TOKEN_INT:
    printf("INT |%.*s| %d\n", (int)tkn->size, tkn->lexeme, tkn->line);
    return;
  case TOKEN_LEFT_BRACE:
    printf("LEFT_BRACE { %d\n", tkn->line);
    return;
  case TOKEN_LEFT_BRACKET:
    printf("LEFT_BRACKET [ %d\n", tkn->line);
    return;
  case TOKEN_LEFT_PAREN:
    printf("LEFT_PAREN ( %d\n", tkn->line);
    return;
  case TOKEN_LESS:
    printf("LESS < %d\n", tkn->line);
    return;
  case TOKEN_LESS_EQUAL:
    printf("LESS_EQUAL <= %d\n", tkn->line);
    return;
  case TOKEN_MINUS:
    printf("MINUS - %d\n", tkn->line);
    return;
  case TOKEN_MINUS_EQUAL:
    printf("MINUS_EQUAL -= %d\n", tkn->line);
    return;
  case TOKEN_MINUS_MINUS:
    printf("MINUS_MINUS -- %d\n", tkn->line);
    return;
  case TOKEN_OR:
    printf("OR || %d\n", tkn->line);
    return;
  case TOKEN_PERCENT:
    printf("PERCENT %% %d\n", tkn->line);
    return;
  case TOKEN_PLUS:
    printf("PLUS + %d\n", tkn->line);
    return;
  case TOKEN_PLUS_EQUAL:
    printf("PLUS_EQUAL += %d\n", tkn->line);
    return;
  case TOKEN_PLUS_PLUS:
    printf("PLUS_PLUS ++ %d\n", tkn->line);
    return;
  case TOKEN_RETURN:
    printf("RETURN return %d\n", tkn->line);
    return;
  case TOKEN_QUESTION:
    printf("QUESTION ? %d\n", tkn->line);
    return;
  case TOKEN_RIGHT_BRACE:
    printf("RIGHT_BRACE } %d\n", tkn->line);
    return;
  case TOKEN_RIGHT_BRACKET:
    printf("RIGHT_BRACKET ] %d\n", tkn->line);
    return;
  case TOKEN_RIGHT_PAREN:
    printf("RIGHT_PAREN ) %d\n", tkn->line);
    return;
  case TOKEN_SLASH:
    printf("SLASH / %d\n", tkn->line);
    return;
  case TOKEN_SLASH_EQUAL:
    printf("SLASH_EQUAL /= %d\n", tkn->line);
    return;
  case TOKEN_STAR:
    printf("STAR * %d\n", tkn->line);
    return;
  case TOKEN_STAR_EQUAL:
    printf("STAR_EQUAL *= %d\n", tkn->line);
    return;
  case TOKEN_STRING:
    printf("STRING \"%.*s\" %d\n", (int)tkn->size, tkn->lexeme, tkn->line);
    return;
  case TOKEN_SWITCH:
    printf("SWITCH switch %d\n", tkn->line);
    return;
  case TOKEN_TRUE:
    printf("TRUE true %d\n", tkn->line);
    return;
  case TOKEN_TYPE:
    printf("TYPE type %d\n", tkn->line);
    return;
  case TOKEN_VAR:
    printf("VAR var %d\n", tkn->line);
    return;
  case TOKEN_WHILE:
    printf("WHILE while %d\n", tkn->line);
    return;
  }
  printf("Received invalid token: %d", tkn->type);
  return;
}
