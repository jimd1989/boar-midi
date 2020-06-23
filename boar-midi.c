#include <err.h>
#include <sndio.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define BUFSIZE 4096

char *parseOnOff(uint8_t *mbuf, char *s) {
  for (; s != '\0' ; s++) {
    if      (*s == 'n') { *mbuf |= 144; return ++s; }
    else if (*s == 'o') { *mbuf |= 128; return ++s; }
  }
  return s;
}

void parseNote(uint8_t *mbuf, char *s) {
  int note = 0;

  s = parseOnOff(mbuf, s);
  if (s == '\0') { mbuf[0] |= 128; mbuf[1] = 0; mbuf[2] = 0; return; }
  note = atoi(s);
  mbuf[1] = note & 127; mbuf[2] = 127;
}

int main(int argc, char **argv) {
  char                  * out           = "rmidi/0";
  char                  * token         = NULL;
  int                     ch            = 0;
  struct mio_hdl        * m             = NULL;
  uint8_t                 mbuf[3]       = {0};
  char                    sbuf[BUFSIZE] = {0};

  if      (argc == 2) { ch = atoi(argv[1]) - 1; }
  else if (argc == 3) { ch = atoi(argv[1]) - 1; out = argv[2]; }
  else                { errx(1, "<MIDI ch> <MIDI port>"); }

  m = mio_open(out, MIO_OUT, false);
  if (m == NULL) { errx(1, "Couldn't find MIDI device %s", out); }
  while (fgets(sbuf, BUFSIZE, stdin) != NULL) {
    token = strtok(sbuf, ";\n");
    while (token != NULL) {
      mbuf[0] = ch;
      parseNote(mbuf, token);
      mio_write(m, mbuf, 3);
      token = strtok(NULL, ";\n");
    }
  }
  return 0;
}
