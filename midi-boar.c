#include <err.h>
#include <poll.h>
#include <sndio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 2048
#define EVENT_MASK 240
#define CHAN_MASK 15
#define NOTE_OFF 128
#define NOTE_ON 144
#define NOT_EVENT(X) ((X) < 128)

typedef struct Settings {
  int   ch;
  int   delay;
} Settings;

Settings readArgs(int argc, char **argv) {
  Settings s = {0};
  char *a = NULL;
  int i = 0;
  s.ch = -1;
  for (i = 1 ; i < argc ; i++) {
    a = argv[i];
    if (strcmp(a, "-delay") == 0)        { s.delay = atoi(argv[++i]); }
    else if (a[0] == '-' && a[1] == 'c') { s.ch = atoi(argv[++i]) - 1; }
    else                                 { errx(1, "Invalid arg %s", a); }
  }
  return s;
}

void wait(struct mio_hdl *m) {
    int nfds, revents = 0;
    struct pollfd pfds[1] = {0};
    do {
        nfds = mio_pollfd(m, pfds, POLLIN);
        if (nfds > 0) {
            if (poll(pfds, nfds, -1) < 0) {
                errx(1, "poll failed");
            }
        }
        revents = mio_revents(m, pfds);
    } while (!(revents & POLLIN));
}

void print(int delay, char onOff, int note) {
    if (delay) { usleep(delay); }
    printf("%c%d\n", onOff, note);
    fflush(stdout);
}

int main(int argc, char **argv) {
  uint8_t buf[BUFSIZE] = {0};
  struct mio_hdl *m;
  int i, event, read, ch, note, vel = 0;
  Settings s = readArgs(argc, argv);

  m = mio_open(MIO_PORTANY, MIO_IN, true);
  if (m == NULL) { errx(1, "Couldn't find MIDI device %s", MIO_PORTANY); }
  while (1) {
    wait(m);
    read = mio_read(m, buf, BUFSIZE);
    for (i = 0; i < read; i++) {
      event = buf[i] & EVENT_MASK;
      ch = buf[i] & CHAN_MASK;
      if ((s.ch == -1) || (s.ch == ch)) {
        if (event == NOTE_ON) {
          note = buf[++i];
          vel = buf[++i];
          if (!vel) { 
            print(s.delay, 'o', note);
          } else {
            print(0, 'n', note | (vel << 9));
          }
        }
        if (event == NOTE_OFF) {
          note = buf[++i];
          if (((i + 1) < read) && NOT_EVENT(buf[i])) {
            vel = buf[++i];
            i++;
          }
          print(s.delay, 'o', note); 
        }
      }
    }
  }
  return 0;
}
