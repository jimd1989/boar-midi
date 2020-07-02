# boar-midi

Convert MIDI on/off messages to [boar](https://github.com/jimd1989/boar) commands, or vice/versa.

## Build

    cc boar-midi.c -lsndio -o boar-midi
    cc midi-boar.c -lsndio -o midi-boar

Requires sndio, which comes installed by default on OpenBSD. Nothing else is supported.

## Usage

### MIDI → Boar

To read MIDI data from a port into a `boar` instance, run

    midi-boar <-input file|-noblock|-delay n|-cn file> | boar

The program will listen to all channels at `rmidi0` by default. Use `-input file` to read from the MIDI device at `file` instead.

Input is blocking. You can increase responsiveness with the `-noblock` flag, but this will require far more computing power.

Input from all 16 MIDI channels will go to stdout unless redirected. An argument of `-cn file` where `n` is a number between 1 and 16 will send all notes found on MIDI channel `n` to `file`.

Some MIDI sequencers send an off signal at the same time as their on signal. If the instrument further down the pipe is having trouble with this, you can delay the printing of an off signal for `n` microseconds with the `-delay n` flag.

### Boar → MIDI

To use the `n` and `o` commands from `boar` as MIDI on and off events respectively, run

    boar -echo-notes | boar-midi <MIDI channel> <MIDI port (optional)>

The program will write to `rmidi0` if a port is not specified. Velocity is ignored for now. On/off events are newline delimited exclusively, like

    n 60
    n 63
    n 67

If you'd prefer to express chords in a compact manner, such as

    n60;n63;n67

then just put

    tr ';' '\n'

somewhere in your pipe. Use the broader Unix environment to add features rather than expecting them from the program itself.

I suspect that there are some edge cases where the parser will fail, but it's unlikely that the input will be hand-written. It should read from `boar -echo-notes` fine.

## Caveats

+ Other MIDI instructions are completely ignored.
+ The program listens for note off signals as note on signals with zero velocity. This is how all my hardware (SQ-1 and JX8P) does it.
+ Conversely, Boar→MIDI encoding sends note off signals in an explicit manner, because it was easier to code.
