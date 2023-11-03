#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

char *read_file(char *file_path) {
  FILE *fp = fopen(file_path, "rb");
  if (fp == NULL) {
    fprintf(stderr, "File \"%s\" not found.\n", file_path);
    exit(1);
  }

  fseek(fp, 0, SEEK_END);
  size_t file_size = ftell(fp);
  char *bytes = malloc(file_size + 1);
  if (bytes == NULL) {
    fclose(fp);
    exit(3);
  }
  rewind(fp);

  size_t bytes_read = fread(bytes, 1, file_size, fp);
  if (file_size != bytes_read) {
    fclose(fp);
    free(bytes);
    exit(1);
  }
  bytes[file_size] = '\0';
  fclose(fp);
  return bytes;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: lainc <file>\n");
    exit(2);
  }
  char *source = read_file(argv[1]);
  init_lexer(source);
  printf("Received source:\n%s", source);
  printf("Tokens:\n");
  token *tkn = get_token();
  while (tkn->type != TOKEN_EOF && tkn->type != TOKEN_ERROR) {
    token_to_string(tkn);
    token_type type = tkn->type;
    if (type == TOKEN_STRING || type == TOKEN_INT || type == TOKEN_INT ||
        type == TOKEN_FLOAT) {
      free(tkn->lexeme);
    }
    free(tkn);
    tkn = get_token();
  }
  token_to_string(tkn);
  free(source);
  free(tkn);
  printf("We finished yay.\n");
  return 0;
}
