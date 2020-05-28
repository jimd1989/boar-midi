# boar-midi

Convert MIDI on/off messages to [boar](https://github.com/jimd1989/boar) commands.

## Build

    cc boar-midi.c -lsndio -o boar-midi

Requires sndio, which comes installed by default on OpenBSD. Nothing else is supported.

## Usage

    boar-midi <-input file|-noblock|-cn file> | boar

The program will listen to all channels at `rmidi0` by default. Use `-input file` to read from the MIDI device at `file` instead.

Input is blocking. You can increase responsiveness with the `-noblock` flag, but this will require far more computing power.

Input from all 16 MIDI channels will go to stdout unless redirected. An argument of `-cn file` where `n` is a number between 1 and 16 will send all notes found on MIDI channel `n` to `file`.

## Caveats

Other MIDI instructions are completely ignored. The program listens for note off signals as note on signals with zero velocity. This is how all my hardware (SQ-1 and JX8P) does it, so I have no reason to implement alternative methods. Please hack this tiny file to your pleasure if you find something lacking.
