#ifndef lexer_h
#define lexer_h

typedef enum {
  TOKEN_AND,
  TOKEN_BANG,
  TOKEN_BOOL,
  TOKEN_EOF,
  TOKEN_EQUAL,
  TOKEN_EQUAL_EQUAL,
  TOKEN_ERROR,
  TOKEN_FLOAT,
  TOKEN_GREATER,
  TOKEN_GREATER_EQUAL,
  TOKEN_INT,
  TOKEN_LEFT_PAREN,
  TOKEN_LESS,
  TOKEN_LESS_EQUAL,
  TOKEN_MINUS,
  TOKEN_OR,
  TOKEN_PLUS,
  TOKEN_RIGHT_PAREN,
  TOKEN_SLASH,
  TOKEN_STAR,
} token_type;

typedef struct {
  token_type type;
  int line;
  char *lexeme;
} token;

void init_lexer(char *source);
token *get_token();

char *type_to_string(token_type t_type) {
  switch (t_type) {
  case TOKEN_AND:
    return "&&";
  case TOKEN_BANG:
    return "!";
  case TOKEN_BOOL:
    return "bool";
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
  case TOKEN_INT:
    return "int";
  case TOKEN_LEFT_PAREN:
    return "(";
  case TOKEN_LESS:
    return "<";
  case TOKEN_LESS_EQUAL:
    return "<=";
  case TOKEN_MINUS:
    return "-";
  case TOKEN_OR:
    return "||";
  case TOKEN_PLUS:
    return "+";
  case TOKEN_RIGHT_PAREN:
    return ")";
  case TOKEN_SLASH:
    return "/";
  case TOKEN_STAR:
    return "*";
  }
  return "invalid token";
}

#endif
