# boar-midi

Convert MIDI on/off messages to [boar](https://github.com/jimd1989/boar) commands.

## Build

  cc boar-midi.c -lsndio -o boar-midi

## Usage

    boar-midi | boar

The program will listen to all channels at `rmidi0` by default and spit all input to stdout. Channel-specific routing is forthcoming. To change the MIDI device or the blocking mode, edit the `mio_open` line accordingly.
