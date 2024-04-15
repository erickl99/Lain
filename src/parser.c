#include "parser.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  unsigned int precedence;
  bool is_left_assoc;
} op_info;

op_info op_info_arr[] = {
    [TOKEN_PLUS] = {1, true},
    [TOKEN_MINUS] = {1, true},
    [TOKEN_STAR] = {2, true},
    [TOKEN_SLASH] = {2, true},
};

bool is_binary_op(token_type type) {
  return type == TOKEN_PLUS || type == TOKEN_MINUS ||
         type == TOKEN_STAR || type == TOKEN_SLASH;
}

bool higher_precedence() {
  return true;
}

static int parse_leaf(token **tkn) {
  if ((*tkn)->type == TOKEN_LEFT_PAREN) {
    int val = parse();
    if ((*tkn)->type != TOKEN_RIGHT_PAREN) {
      fprintf(stderr, "Unmatched '('\n");
    }
    *tkn = get_token();
    return val;
  }
  if ((*tkn)->type == TOKEN_INT) {
    int val = atoi((*tkn)->lexeme);
    *tkn = get_token();
    return val;
  }
  fprintf(stderr, "Expected integer leaf\n");
  return -1;
}

int parse() {
  token *tkn = get_token();
  int lhs = parse_leaf(&tkn);
  if (lhs == -1) {
    return -1;
  }
  for (;;) {
    if (tkn->type == TOKEN_EOF || !is_binary_op(tkn->type)) {
      break;
    }
  }
  return 0;
}
