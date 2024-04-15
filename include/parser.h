#ifndef parser_h
#define parser_h

#include <stdbool.h>

typedef enum {
  BINARY_EXPR,
  LITERAL_EXPR,
  LOGICAL_EXPR,
  UNARY_EXPR
} node_type;

typedef struct {
  node_type of;
} ln_expr;

int parse();

#endif
