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
#define POLY_AFTERTOUCH 160
#define CC 176
#define PROGRAM_CHANGE 192
#define CHAN_AFTERTOUCH 208
#define PITCH_BEND 224
#define NOT_EVENT(X) ((X) < 128)
#define SYSEX_START 240
#define SYSEX_END 247
#define MTC 241
#define SONG_POSITION 242
#define SONG_SELECT 243
#define TUNE_REQUEST 246
#define REAL_TIME_EVENT 248

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
  bool inSysex = false;
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
      if (buf[i] == SYSEX_END) { inSysex = false; }
      else if (inSysex) { continue; }
      else if ((event == NOTE_ON) || (event == NOTE_OFF)) {
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
          else if (event == NOTE_OFF) {
            note = buf[++i];
            if (((i + 1) < read) && NOT_EVENT(buf[i])) {
              vel = buf[++i];
            }
            print(s.delay, 'o', note); 
          }
        }
      } 
      else if (event == POLY_AFTERTOUCH)  { i += 2; }
      else if (event == CC)               { i += 2; }
      else if (event == PROGRAM_CHANGE)   { i ++; }
      else if (event == CHAN_AFTERTOUCH)  { i ++; }
      else if (event == PITCH_BEND)       { i += 2; }
      else if (buf[i] == MTC)             { i ++; }
      else if (buf[i] == SONG_POSITION)   { i += 2; }
      else if (buf[i] == SONG_SELECT)     { i ++; }
      else if (buf[i] == PITCH_BEND)      { i += 2; }
      else if (buf[i] == TUNE_REQUEST)    { continue; }
      else if (buf[i] == SYSEX_START)     { inSysex = true; }
      else if (buf[i] >= REAL_TIME_EVENT) { continue; }
      else                                { continue; }
    }
  }
  return 0;
}
