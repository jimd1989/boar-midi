#include <stdio.h>

#define BUFSIZE 4096

int main () {
  char buf[BUFSIZE] = {0};
  char *c = NULL;
  while (fgets(buf, BUFSIZE, stdin) != NULL) {
    for (c = buf ; *c != '\0' ; c++) {
      if (*c == ';') { *c = '\n'; }
    }
    printf("%s", buf);
    fflush(stdout);
  }
  return 0;
}
