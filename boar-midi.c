#include <err.h>
#include <sndio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 64

typedef struct Settings {
  char        * in;  
  FILE        * out[16];
  bool          noblock;
} Settings;

Settings setChannel(Settings s, char *a, char *f) {
  int ch = atoi(a);
  if (ch < 1 || ch > 16) { errx(1, "Invalid channel %d", ch); }
  s.out[ch - 1] = fopen(f, "w");
  if (s.out[ch - 1] == NULL) { errx(1, "Failed to open %s", f); }
  return s;
}

Settings readArgs(int argc, char **argv) {
  Settings s = {0};
  char *a = NULL;
  int i = 0;
  s.in = "rmidi/0";
  for (; i < 16; i++) { s.out[i] = stdout; }
  for (i = 1 ; i < argc ; i++) {
    a = argv[i];
    if      (strcmp(a, "-noblock") == 0) { s.noblock = true; }
    else if (strcmp(a, "-input") == 0)   { s.in = argv[++i]; }
    else if (a[0] == '-' && a[1] == 'c') { s = setChannel(s, a + 2, argv[++i]); }
    else                                 { errx(1, "Invalid arg %s", a); }
  }
  return s;
}

int main(int argc, char **argv) {
  uint8_t buf[BUFSIZE] = {0};
  struct mio_hdl *m;
  int i, read, ch, note, velocity = 0;
  Settings s = readArgs(argc, argv);

  m = mio_open(s.in, MIO_IN, s.noblock);
  if (m == NULL) { errx(1, "Couldn't find MIDI device %s", s.in); }
  while (1) {
    read = mio_read(m, buf, BUFSIZE); 
    for (i = 0; i < read; i++) {
      ch = 144^buf[i];
      if (ch < 16) {
        note = buf[++i];
        velocity = buf[++i];
        if (!velocity) { fprintf(s.out[ch], "o%d\n", note); }
        else           { fprintf(s.out[ch], "n%d\n", note | (velocity << 9)); }
      }
    }
    fflush(stdout);
  }
  return 0;
}
