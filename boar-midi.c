#include <stdio.h>
#include <sndio.h>

#define BUFSIZE 64

int main() {
    uint8_t buf[BUFSIZE] = {0};
    struct mio_hdl *m;
    int i, read, channel, note, velocity = 0;

    m = mio_open("rmidi/0", MIO_IN, 0);
    if (m == NULL) {
        printf("Couldn't find MIDI device.\n");
        return 1;
    }
    while (1) {
        read = mio_read(m, buf, BUFSIZE); 
        i = 0;
        while (i < read) {
            channel = 144^buf[i];
            if (channel < 16) {
                note = buf[++i];
                velocity = buf[++i];
                if (!velocity) {
                    printf("o%d\n", note);
                } else {
                    printf("n%d\n", note | (velocity << 9));
                }
                fflush(stdout);
            }
            i++;
        }
    }
    return 0;
}
