#include "lexer.h"
#include <stddef.h>
#include <stdbool.h>

typedef struct {
  char *curr;
  int line;
} lexer;

lexer glexer;

void init_lexer(char *source) { glexer.curr = source; };


static char advance() { return *glexer.curr++; }

static void skip_whitespace() {
  for (;;) {
    switch (advance()) {
    case '\n':
      glexer.line++;
    }
  }
}

token *get_token() {
  for (;;) {
    skip_whitespace();
    switch (advance()) {}
  }
}
